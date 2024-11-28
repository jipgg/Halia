#pragma once
#include <lualib.h>
#include <optional>
#include <string>
#include <boost/process/v1/child.hpp>
struct System_exit_callback{
    std::optional<std::string> std_out;
    std::optional<std::string> std_err;
    int exit_code;
    bool success() const {return exit_code == 0;}
};
using Child_process = boost::process::child;
namespace exported {
void init_process_callback_meta(lua_State* L);
void init_child_meta(lua_State* L);
}
