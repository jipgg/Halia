#include "library.h"
#include <lualib.h>
#include "common/common.h"
#include "init.h"
#include "common/userdata_helpers.h"
#include <boost/process.hpp>
#include <iostream>
using namespace std::string_literals;
namespace bp = boost::process;

static int system(lua_State* L) {
    std::string command{luaL_checkstring(L, 1)};
    if (const int top = lua_gettop(L); top > 1) {
        for (int i{2}; i < lua_gettop(L); ++i) {
            command += " "s + luaL_checkstring(L, i);
        }
    }
    bp::ipstream stdout_stream;
    bp::ipstream stderr_stream;
    int exit_code = bp::system(luaL_checkstring(L, 1),
        bp::std_out > stdout_stream, bp::std_err > stderr_stream);
    std::string line_dummy;
    std::string stdout_string;
    while(std::getline(stdout_stream, line_dummy)) stdout_string += line_dummy;
    std::string stderr_string;
    while (std::getline(stderr_stream, line_dummy)) stderr_string += line_dummy;
    create<ProcessCallback>(L, ProcessCallback{
        .std_out = stdout_string.size() ?
            std::make_optional(std::move(stdout_string)):
            std::nullopt,
        .std_err = stderr_string.size() ?
            std::make_optional(std::move(stderr_string)):
            std::nullopt,
        .exit_code = exit_code,
    });
    return 1;
}


static const luaL_Reg functions[] = {
    {"system", system},
    {nullptr, nullptr}
};

namespace library {
BuiltinLibrary process{"process", [](lua_State* L) {
    exported::init_process_callback_meta(L);
    lua_newtable(L);
    luaL_register(L, nullptr, functions);
    return 1;
}};
}
