#include "module.hpp"
#include "common/metamethod.hpp"
#include "common/NamecallAtom.hpp"
#include "type_utils.hpp"
#include <boost/process.hpp>
#include <variant>
constexpr const char* type = "Child";
constexpr std::string_view pid_key = "pid";
constexpr std::string_view valid_key = "valid";
constexpr std::string_view exit_code_key = "exit_code";
constexpr std::string_view native_exit_code_key = "native_exit_code";
using namespace std::string_literals;
namespace bp = boost::process;
template <class ...Ts>
static std::variant<Child, std::string> execute(const std::string& exe, Ts&&...args) {
    bp::v1::filesystem::path executable = bp::search_path(exe);
    if (executable == "") return std::string("executable not found.");
    try {
        Child child = bp::child(executable, args...);
        return child;
    } catch(std::exception& e) {
        return std::string(e.what());
    }
}
static int index(lua_State* L) {
    auto& self = check<Child>(L, 1);
    const std::string_view key = luaL_checkstring(L, 2);
    switch (key[0]) {
        case pid_key[0]:
            if (key == pid_key) {
                create<ProcessID>(L, self.id());
                return 1;
            }
        case valid_key[0]:
            if (key == valid_key) {
                lua_pushboolean(L, self.valid());
                return 1;
            }
        case exit_code_key[0]:
            if (key == exit_code_key) {
                lua_pushinteger(L, self.exit_code());
                return 1;
            }
        case native_exit_code_key[0]:
            if (key == native_exit_code_key) {
                lua_pushinteger(L, self.native_exit_code());
            }
    }
    luaL_errorL(L, "invalid index");
    return 0;
}
static int newindex(lua_State* L) {
    luaL_errorL(L, "indices are readonly.");
    return 0;
}
static int tostring(lua_State* L) {
    const auto& self = check<Child>(L, 1);
    std::string parse = std::string(type) + "{\n";
    parse += "  .pid = \"" + std::to_string(static_cast<size_t>(self.id())) + "\",\n";
    parse += "  .valid = "s + (self.valid() ? "true" : "false") + ",\n";
    parse += "  .exit_code = "s + std::to_string(self.exit_code()) + ",\n";
    parse += "  .native_exit_code = "s + std::to_string(self.native_exit_code()) + ",\n";
    parse += "}";
    lua_pushstring(L, parse.c_str());
    return 1;
}
static int namecall(lua_State* L) {
    auto& self = check<Child>(L, 1);
    int atom;
    lua_namecallatom(L, &atom);
    using A = NamecallAtom;
    switch (static_cast<A>(atom)) {
        case A::join:
            self.join();
            return 0;
        case A::detach:
            self.detach();
            return 0;
        case A::wait:
            self.wait();
            return 0;
        case A::joinable:
            lua_pushboolean(L, self.joinable());
            return 1;
        case A::running:
            lua_pushboolean(L, self.running());
            return 1;
        case A::terminate:
            self.terminate();
            return 0;
        default:
            luaL_errorL(L, "invalid method");
            return 0;
    }
}
static const luaL_Reg meta[] = {
    {metamethod::index, index},
    {metamethod::newindex, newindex},
    {metamethod::tostring, tostring},
    {metamethod::namecall, namecall},
    {nullptr, nullptr}
};
void module::process::init_child_process_meta(lua_State* L) {
    if (luaL_newmetatable(L, metatable_name<Child>())) {
        luaL_register(L, nullptr, meta);
        lua_pushstring(L, type);
        lua_setfield(L, -2, metamethod::type);
    }
    lua_pop(L, 1);
}
int module::process::child_process_ctor(lua_State* L) {
    const std::string exe = luaL_checkstring(L, 1);
    std::string args = luaL_checkstring(L, 2);
    bool silent = false;
    for (int i{3}; i <= lua_gettop(L); ++i) {
        //if (is_type<Process_option>(L, i)) {
            //if (check<Process_option>(L, i) == Process_option::silent) silent = true;
        //} else if (lua_isstring(L, i)) args = args.empty() ? lua_tostring(L, i) : args + " "s + lua_tostring(L, i);
        args += luaL_checkstring(L, i);
    }
    auto exe_path = bp::search_path(exe);
    if (exe_path == "") {
        luaL_argerrorL(L, 1, "executable path does not exist.");
        return 0;
    }
    if (silent) {
        create<Child>(L, exe_path, args, bp::std_out > bp::null);
    } else {
        create<Child>(L, exe_path, args);
    }
    return 1;
}
