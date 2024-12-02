#pragma once
#include <type_utils.hpp>
#include <lualib.h>
#include <optional>
#include <string>
#define _WIN32_WINNT 0x0601
#include <boost/process.hpp>
using namespace halia;
struct Execution_feedback{
    std::optional<std::string> output;
    std::optional<std::string> error;
    int exit_code;
    bool failed_before_execution;
};
using Child_process = boost::process::child;
using Process_id = boost::process::pid_t;
enum class Process_option {
    silent,
};
namespace exported {
void init_execution_feedback_meta(lua_State* L);
void init_child_process_meta(lua_State* L);
void init_pid_meta(lua_State* L);
int child_process_ctor(lua_State* L);
}
