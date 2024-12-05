#pragma once
#include <type_utils.hpp>
#include <lualib.h>
#include <optional>
#include <string>
#include <variant>
#include <span>
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
struct Read_buffer {
    boost::process::ipstream stream;
    std::optional<std::string> getline() {
        std::string line;
        if (std::getline(stream, line)) return line;
        return std::nullopt;
    }
};
struct Process {
    std::unique_ptr<boost::process::child> child;
    Read_buffer cout;
    Read_buffer cerr;
};
template <class ...Ts>
std::variant<Process, std::string> create_process(const std::string& exe, Ts&&...args) {
    namespace bp = boost::process;
    bp::v1::filesystem::path executable = bp::search_path(exe);
    if (executable == "") return std::string("executable not found.");
    try {
        Process process = Process{};
        process.child = std::make_unique<bp::child>(
            executable,
            args...,
            bp::std_out > process.cout.stream,
            bp::std_err > process.cerr.stream
        );
        return process;
    } catch(std::exception& e) {
        return std::string(e.what());
    }
}
using Process_id = boost::process::pid_t;
using Args_span = std::span<std::string_view>;
enum class Process_option {
    silent,
};
namespace module::process {
void init_execution_feedback_meta(lua_State* L);
void init_child_process_meta(lua_State* L);
void init_args_span_meta(lua_State* L);
void init_pid_meta(lua_State* L);
int child_process_ctor(lua_State* L);
void init_process_meta(lua_State* L);
}
