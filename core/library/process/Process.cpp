#include "module.hpp"
#include "common/metamethod.hpp"
#include "common/NamecallAtom.hpp"
#include "type_utils.hpp"
#include <boost/process.hpp>
constexpr const char* type = "Process";
using sv_t = std::string_view;
constexpr sv_t pid_key = "pid";
constexpr sv_t valid_key = "valid";
constexpr sv_t exit_code_key = "exit_code";
constexpr sv_t native_exit_code_key = "native_exit_code";
constexpr sv_t stdout_key = "stdout"; 
constexpr sv_t stderr_key = "stderr"; 
using namespace std::string_literals;
static int index(lua_State* L) {
    auto& self = check<Process>(L, 1);
    auto& child = self.child;
    if (child == nullptr) {
        luaL_errorL(L, "child process was nil.");
        return 0;
    }
    const std::string_view key = luaL_checkstring(L, 2);
    switch (key[0]) {
        case pid_key[0]:
            if (key == pid_key) {
                create<ProcessID>(L, child->id());
                return 1;
            }
        case valid_key[0]:
            if (key == valid_key) {
                lua_pushboolean(L, child->valid());
                return 1;
            }
        case exit_code_key[0]:
            if (key == exit_code_key) {
                lua_pushinteger(L, child->exit_code());
                return 1;
            }
        case native_exit_code_key[0]:
            if (key == native_exit_code_key) {
                lua_pushinteger(L, child->native_exit_code());
                return 1;
            }
        case stdout_key[0]:
            if (key == stdout_key) {
                push<ReadBuffer>(L, self.cout);
                return 1;
            } else if (key == stderr_key) {
                push<ReadBuffer>(L, self.cerr);
                return 1;
            }
    }
    luaL_errorL(L, "invalid index");
    return 0;
}
static int newindex(lua_State* L) {
    luaL_errorL(L, "indices are readonly.");
    return 0;
}
static int namecall(lua_State* L) {
    auto& self = check<Process>(L, 1);
    auto& child = self.child;
    if (child == nullptr) {
        luaL_errorL(L, "child process was nil.");
        return 0;
    }
    int atom;
    lua_namecallatom(L, &atom);
    using A = NamecallAtom;
    switch (static_cast<A>(atom)) {
        case A::join:
            child->join();
            return 0;
        case A::detach:
            child->detach();
            return 0;
        case A::wait:
            child->wait();
            return 0;
        case A::joinable:
            lua_pushboolean(L, child->joinable());
            return 1;
        case A::running:
            lua_pushboolean(L, child->running());
            return 1;
        case A::terminate:
            child->terminate();
            return 0;
        default:
            luaL_errorL(L, "invalid method");
            return 0;
    }
}
static const luaL_Reg meta[] = {
    {metamethod::index, index},
    {metamethod::newindex, newindex},
    {metamethod::namecall, namecall},
    {nullptr, nullptr}
};
void module::process::init_process_meta(lua_State* L) {
    if (luaL_newmetatable(L, metatable_name<Process>())) {
        luaL_register(L, nullptr, meta);
        lua_pushstring(L, type);
        lua_setfield(L, -2, metamethod::type);
    }
    lua_pop(L, 1);
}
