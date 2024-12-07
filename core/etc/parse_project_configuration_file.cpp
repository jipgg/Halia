#include "ProjectConfiguration.hpp"
#include <lualib.h>
#include <Luau/Compiler.h>
#include <luacode.h>
#include "common.hpp"
#include <format>
#include <variant>
using Runtime = halia::ProjectConfiguration::Runtime;
using Flags = halia::ProjectConfiguration::Flags;
namespace fs = std::filesystem;
constexpr std::string_view luau_extension = ".luau";
using halia::ErrorInfo;

[[nodiscard]] static bool is_special_stem(const std::string& stem) {
    if (stem.find(".project") != std::string::npos) {
        return true;
    }
    if (stem.find(".config") != std::string::npos) {
        return true;
    }
    return false;
}

[[nodiscard]] static std::optional<fs::path> find_script_entry_point(const fs::path& path) {
    if (fs::is_directory(path) and fs::exists(path / "init.luau")) {
        return path / "init.luau";
    }
    if (fs::is_regular_file(path) and path.extension() == luau_extension) {
        if (is_special_stem(path.stem().string())) return std::nullopt;
        return path;
    }
    return std::nullopt;
}
[[nodiscard]] static std::optional<ErrorInfo> resolve_and_emplace_scripts(const fs::path& path, std::vector<fs::path>& to) {
    const size_t old_size = to.size();
    if (not fs::exists(path)) return ErrorInfo{std::format("path does not exist '{}'", path.string())};
    if (fs::is_regular_file(path)) {
        printerr(path.extension());
        to.emplace_back(path);
        return std::nullopt;
    }
    if (fs::is_directory(path)) {
        bool emplaced_once = false;
        for (auto& entry : fs::directory_iterator(path)) {
            const fs::path path_entry = entry.path();
            printerr(path_entry, path_entry.extension(), path_entry.stem());
            if (auto found = find_script_entry_point(path_entry)) {
                to.emplace_back(std::move(*found));
                emplaced_once = true;
            }
        }
        return std::nullopt;
    }
    if (to.size() == old_size) {
        return ErrorInfo{std::format("no source files given for '{}'", path.string())};
    }
    return std::nullopt;
}
struct RaiiState {
    std::unique_ptr<lua_State, decltype(&lua_close)> ptr;
    explicit RaiiState(lua_State* L): ptr(L, lua_close) {}
    operator lua_State*() const {return ptr.get();}
};
struct LoadedConfigInfo {
    RaiiState state;
    fs::path base_path;
    int ref;
};
[[nodiscard]] static std::variant<LoadedConfigInfo, ErrorInfo> load_config(const fs::path& config_file) {
    auto base_path = config_file.parent_path();
    RaiiState state{luaL_newstate()};
    lua_State* L = state;
    auto source = read_file(config_file);
    if (not source) return ErrorInfo("unable to read source");
    std::string bytecode = Luau::compile(*source);
    if (luau_load(L, "project config file", bytecode.data(), bytecode.size(), 0) != LUA_OK) {
        return ErrorInfo(bytecode);
    }
    if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
        std::string errmsg = lua_tostring(L, -1);
        lua_pop(L, 1);
        return ErrorInfo(errmsg);
    }
    if (not lua_istable(L, -1)) {
        return ErrorInfo{"not a table"};
    }
    int ref = lua_ref(L, -1);
    return LoadedConfigInfo{.state = std::move(state), .base_path = base_path, .ref = ref};
}

namespace halia {
std::variant<ProjectConfiguration, ErrorInfo> parse_project_configuration_file(const std::filesystem::path& config_file) {
    auto result = load_config(config_file);
    if (ErrorInfo* error_info = std::get_if<ErrorInfo>(&result)) {
        return error_info->propagate();
    }
    auto& a = std::get<LoadedConfigInfo>(result);
    lua_State* L = a.state;
    ProjectConfiguration config{};
    lua_getfield(L, -1, "runtime");
    if (not lua_isstring(L, -1)) {
        lua_pop(L, 1);
        return ErrorInfo{"index 'runtime' must be a valid value"};
    }
    const std::string_view runtime = lua_tostring(L, -1);
    if (runtime == "desktop") config.runtime = Runtime::desktop;
    else if (runtime == "console") config.runtime = Runtime::console;
    lua_pop(L, 1);
    lua_getfield(L, -1, "includes");
    if (lua_isstring(L, -1)) {
        fs::path script_path{a.base_path / lua_tostring(L, -1)};
        if (auto err = resolve_and_emplace_scripts(script_path, config.script_paths)) return err->propagate();
        lua_pop(L, 1);
    } else if (lua_istable(L, -1)) {
        const int len = lua_objlen(L, -1);
        for (int i{1}; i <= len; ++i) {
            lua_rawgeti(L, -1, i);
            if (not lua_isstring(L, -1)) {
                lua_pop(L, 1);
                return ErrorInfo{std::format("element at index '{}' was not a string.", i)};
            }
            if (auto err = resolve_and_emplace_scripts(a.base_path / lua_tostring(L, -1), config.script_paths)) return err->propagate();
            lua_pop(L, 1);
        }
    } else {
        return ErrorInfo{"no valid source files given"};
    }
    return config;
}
}
