#include "library.hpp"
#include <lualib.h>
#include "common/common.hpp"
#include "init.hpp"
#include <boost/process.hpp>
#include <std/filesystem/init.hpp>
using namespace std::string_literals;
namespace bp = boost::process;
template <class...Ts>
static Execution_feedback execute(const std::string& exe, Ts&&...args) {
    bp::ipstream out_stream;
    bp::ipstream err_stream;
    bp::v1::filesystem::path executable = bp::search_path(exe);
    int exit_code{-1};
    try {
        exit_code = bp::system(bp::search_path(exe), args..., bp::std_out > out_stream, bp::std_err > err_stream);
    } catch (std::exception& e) {
        return {
            .output = std::nullopt,
            .error = e.what(),
            .exit_code = exit_code,
            .failed_before_execution = true,
        };
    }
    std::string line;
    std::string out;
    while(out_stream and std::getline(out_stream, line)) out += line + '\n';
    std::string err;
    while(err_stream and std::getline(err_stream, line)) err += line + '\n';
    return Execution_feedback{
        .output = out.empty() ? std::nullopt : std::make_optional<std::string>(out),
        .error = err.empty() ? std::nullopt : std::make_optional<std::string>(err),
        .exit_code = exit_code,
        .failed_before_execution = false,
    };
}
static int exists_in_path_environment(lua_State* L) {
    auto found = bp::search_path(luaL_checkstring(L, 1));
    lua_pushboolean(L, found != "");
    return 1;
}
static int find_in_path_environment(lua_State* L) {
    auto found = bp::search_path(luaL_checkstring(L, 1));
    if (found != "") {
        lua_pushstring(L, bp::v1::filesystem::absolute(found).string().c_str());
    } else lua_pushnil(L);
    return 1;
}

static int execute_command(lua_State* L) {
    const int top = lua_gettop(L);
    if (top == 0) luaL_argerrorL(L, 1, nullptr);
    const std::string exe{luaL_checkstring(L, 1)};
    if (top == 1) {
        halia::create<Execution_feedback>(L, execute(exe));
        return 1;
    }
    std::string command{luaL_checkstring(L, 2)};
    for (int i{3}; i <= top; ++i) command += " "s + luaL_checkstring(L, i);
    halia::create<Execution_feedback>(L, execute(exe, command));
    return 1;
}
static const luaL_Reg functions[] = {
    {"execute", execute_command},
    {"exists_in_path_environment", exists_in_path_environment},
    {"find_in_path_environment", find_in_path_environment},
    {nullptr, nullptr}
};

namespace library {
Builtin_library process{"process", [](lua_State* L) {
    exported::init_process_callback_meta(L);
    lua_newtable(L);
    luaL_register(L, nullptr, functions);
    return 1;
}};
}
