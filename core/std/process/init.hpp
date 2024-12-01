#pragma once
#include <type_utils.hpp>
#include <lualib.h>
#include <optional>
#include <string>
#include <boost/process/v1/child.hpp>
using namespace halia;
struct Execution_feedback{
    std::optional<std::string> output;
    std::optional<std::string> error;
    int exit_code;
    bool failed_before_execution;
};
using Child = boost::process::child;
namespace exported {
void init_process_callback_meta(lua_State* L);
void init_child_meta(lua_State* L);
}
