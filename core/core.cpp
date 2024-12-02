#include "core.hpp"
#include "type_utils.hpp"
#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#include <luacode.h>
#include <luacodegen.h>
#include "builtin.hpp"
#include <Luau/Compiler.h>
#include "common/Namecall_atom.hpp"
#include "lua_base.hpp"
#include "common/comptime_enum.hpp"
#include "common/common.hpp"
#include <queue>
#include <stdexcept>
#include "Cothread.hpp"
#include "library.hpp"
#include <string_view>
namespace fs = std::filesystem;
static constexpr auto builtin_name = "std";
static lua_State* main_state;
static fs::path bin_path;
static std::span<std::string_view> args;
class Cothread_scheduling {
    std::unordered_map<lua_State*, Cothread> registry_;
    std::queue<std::reference_wrapper<Cothread>> queue_;
public:
    Cothread& emplace(Cothread&& co) {
        print(std::source_location::current().line(), "COTHREAD EMPLACED:");
        auto it = registry_.insert({co.state(), std::move(co)});
        return queue_.emplace(it.first->second);
    }
    void pop() noexcept {
        lua_State* key = queue_.front().get().state();
        queue_.pop();
        registry_.erase(key);
    }
    bool empty() const noexcept {
        return queue_.empty();
    }
    Cothread* find(lua_State* thread_state) noexcept {
        auto found = registry_.find(thread_state);
        if (found != registry_.end()) return &found->second;
        return nullptr;
    }
    void requeue() {
        queue_.push(queue_.front());
        queue_.pop();
    }
    Cothread& front() noexcept {
        return queue_.front();
    }
    Cothread& back() noexcept {
        return queue_.back();
    }
} static cothreads;
using Unique_event = std::unique_ptr<builtin::Event>; 
static void register_event(lua_State* L, Unique_event& ev, const char* fieldname) {
    ev = std::make_unique<builtin::Event>(L);
    halia::push(L, *ev);
    lua_setfield(L, -2, fieldname);
}

static fs::path res_path() {
    return bin_path / "resources/luau_library";
}
static void register_builtin_library(lua_State* L, Builtin_library& module) {
    module.load(L);
    lua_setfield(L, -2, module.name);
}

struct Core_error {
    enum class Type {
        runtime,
        compile,
        load,
        file_not_found,
        comptime_SENTINEL
    };
    Type type;
    std::string message;
    constexpr int exit_code() {return -static_cast<int>(type) - 1;}
};
static bool process_threads(lua_State* L) {
    if (cothreads.empty()) {
        return false;
    }
    using Status = Cothread::Status;
    Cothread& proc = cothreads.front();
    lua_State* TL = proc.state();
    int status = lua_status(TL);
    if (status == LUA_YIELD and proc.status == Status::waiting) {
        if (std::chrono::steady_clock::now() - proc.yield_start >= proc.yield_duration) {
            status = lua_resume(TL, L, 0);
        } else {
            cothreads.requeue();
            return true;
        }
    }
    if (status == LUA_OK) {
        proc.status = Status::finished;
    } else if (status != LUA_YIELD) {
        proc.status = Status::had_error;
        proc.error_message = luaL_checkstring(TL, -1);
    }
    cothreads.pop();
    return !cothreads.empty();
}
static std::optional<Core_error> run_script(lua_State* L, const fs::path& script) {
    std::optional<std::string> source = read_file(script);
    using namespace std::string_literals;
    if (not source) {
        return Core_error{
            .type = Core_error::Type::file_not_found,
            .message = "Unable to read file '" + script.string() + "'",
        };
    }
    auto identifier = script.filename().string();
    identifier = "=" + identifier;
    std::string bytecode = Luau::compile(*source, compile_options());
    Cothread proc{L};
    lua_State* TL = proc.state();
    luaL_sandboxthread(TL);
    if (luau_load(TL, identifier.c_str(), bytecode.data(), bytecode.size(), 0)) {
        Core_error err{
            .type = Core_error::Type::load,
            .message = "Build error:"s + luaL_checkstring(TL, -1),
        };
        return err;
    }
    int status = lua_pcall(TL, 0, 0, 0);
    if (status == LUA_OK) {
        return std::nullopt;
    } 
    if (status == LUA_YIELD) {
        cothreads.emplace(std::move(proc));
        return std::nullopt;
    }
    return Core_error{
        .type = Core_error::Type::runtime,
        .message = luaL_checkstring(TL, -1)
    };
}
static std::optional<Core_error> init_luau_state(const fs::path& main_entry_point) {
    luaL_openlibs(main_state);
    lua_callbacks(main_state)->useratom = [](const char* raw_name, size_t s) {
        std::string_view name{raw_name, s};
        static constexpr auto count = static_cast<size_t>(Namecall_atom::_last);
        try {
            auto e = comptime_enum::item<Namecall_atom, count>(name);
            return static_cast<int16_t>(e.index);
        } catch (std::out_of_range& e) {
            luaL_error(main_state, e.what());
            return 0i16;
        }
    };
    lua_register_globals(main_state);
    lua_newtable(main_state);
    lua_setglobal(main_state, builtin_name);
    builtin::register_event_type(main_state);
    lua_getglobal(main_state, builtin_name);
    register_builtin_library(main_state, library::filesystem);
    register_builtin_library(main_state, library::math);
    register_builtin_library(main_state, library::process);
    register_builtin_library(main_state, library::task);
    lua_pop(main_state, 1);
    try {
    auto unexpected = run_script(main_state, main_entry_point);
    if (unexpected) return unexpected;
    } catch (std::exception& e) {
        printerr(e.what());
    }
    luaL_sandbox(main_state);
    print("init_state is ok");
    return std::nullopt;
}
static std::optional<Core_error> init(halia::core::Launch_options opts) {
    args = std::move(opts.args);
    main_state = luaL_newstate();
    bin_path = std::move(opts.bin_path);
    auto unexpected = init_luau_state(opts.main_entry_point);
    if (unexpected) return unexpected;
    print("init is ok");
    return std::nullopt;
}
namespace halia {
namespace intern {
int unique_tag_incr{0};
std::unordered_map<std::string, int> type_registry{};
}
namespace core{
void emplace_cothread(Cothread&& co) {
    cothreads.emplace(std::move(co));
}
Cothread* find_cothread(lua_State* thread_state) noexcept {
    return cothreads.find(thread_state);
}
int bootstrap(Launch_options opts) {
    auto unexpected = init(opts);
    if (unexpected) {
        lua_close(main_state);
        constexpr auto arr = comptime_enum::to_array<Core_error::Type>();
        const int error_type = static_cast<int>(unexpected->type);
        for (const auto& v : arr) if (v.index == error_type) {
            printerr(std::format("Error ({}): {}", v.name, unexpected->message));
            return unexpected->exit_code();
        }
        printerr("Weird error");
        return unexpected->exit_code();
    }
    while(!cothreads.empty()) {
        process_threads(main_state);
    }
    lua_close(main_state);
    return 0;
}
void add_library(const Library_entry &entry) {
    Builtin_library lib(entry.name, entry.loader);
    lua_getglobal(main_state, builtin_name);
    register_builtin_library(main_state, lib);
    lua_pop(main_state, 1);
}
lua_State* lua_state() noexcept {
    return main_state;
}
std::span<std::string_view> args_span() noexcept {
    return args;
}
}
}
