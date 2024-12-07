#include "module.hpp"
#include "library.hpp"
#include <lualib.h>
#include <boost/process.hpp>
#include "core.hpp"
using namespace std::string_literals;
namespace bp = boost::process;
static std::string ipstream_to_string(bp::ipstream& ipstream) {
    std::string string;
    std::string line;
    while (std::getline(ipstream, line)) string += line + '\n';
    return string;
}
static int run(lua_State* L) {
    constexpr std::string_view capture_stdout = "stdout";
    constexpr std::string_view capture_stderr = "stderr";
    constexpr std::string_view capture_stdout_stderr = "stdout | stderr";
    constexpr std::string_view capture_stdout_stderr_merged = "stdout & stderr";
    constexpr std::string_view capture_null = "null";
    if (not lua_isstring(L, 2)) {
        const int exit_code = bp::system(luaL_checkstring(L, 1));
        lua_pushinteger(L, exit_code);
        return 1;
    }
    const std::string_view capture = luaL_checkstring(L, 2);
    if (capture == capture_stdout) {
        bp::ipstream out;
        const int exit_code = bp::system(luaL_checkstring(L, 1), bp::std_out > out, bp::std_err > bp::null);
        lua_pushinteger(L, exit_code);
        lua_pushstring(L, ipstream_to_string(out).c_str());
        return 2;
    } else if (capture == capture_stderr) {
        bp::ipstream err;
        const int exit_code = bp::system(luaL_checkstring(L, 1), bp::std_out.null(), bp::std_err > err);
        lua_pushinteger(L, exit_code);
        lua_pushstring(L, ipstream_to_string(err).c_str());
        return 2;
    } else if (capture == capture_stdout_stderr) {
        bp::ipstream out;
        bp::ipstream err;
        const int exit_code = bp::system(luaL_checkstring(L, 1), bp::std_out > out, bp::std_err > err);
        lua_pushinteger(L, exit_code);
        lua_pushstring(L, ipstream_to_string(out).c_str());
        lua_pushstring(L, ipstream_to_string(err).c_str());
        return 3;
    } else if (capture == capture_null) {
        const int exit_code = bp::system(luaL_checkstring(L, 1), (bp::std_out & bp::std_err) > bp::null);
        lua_pushinteger(L, exit_code);
        return 1;
    } else if (capture == capture_stdout_stderr_merged) {
        bp::ipstream merged;
        const int exit_code = bp::system(luaL_checkstring(L, 1), (bp::std_out & bp::std_err) > merged);
        lua_pushinteger(L, exit_code);
        lua_pushstring(L, ipstream_to_string(merged).c_str());
        return 2;
    }
    luaL_argerrorL(L, 2, "invalid capture option");
    return 0;
}
template <class...Ts>
static ExecutionFeedback execute(const std::string& exe, Ts&&...args) {
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
    return ExecutionFeedback{
        .output = out.empty() ? std::nullopt : std::make_optional<std::string>(out),
        .error = err.empty() ? std::nullopt : std::make_optional<std::string>(err),
        .exit_code = exit_code,
        .failed_before_execution = false,
    };
}
static int create(lua_State* L) {
    const int top = lua_gettop(L);
    if (top == 0) {
        luaL_argerrorL(L, 1, "no executable provided");
        return 0;
    }
    auto exe = bp::search_path(luaL_checkstring(L, 1));
    if (exe == "") {
        luaL_argerrorL(L, 1, "couldn't find executable.");
        return 0;
    }
    if (top == 1) {
        auto result = create_process(exe.string());
        if (auto* p = std::get_if<Process>(&result)) {
            create<Process>(L, std::move(*p));
            return 1;
        }
    }
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
        halia::create<ExecutionFeedback>(L, execute(exe));
        return 1;
    }
    std::string command{luaL_checkstring(L, 2)};
    for (int i{3}; i <= top; ++i) command += " "s + luaL_checkstring(L, i);
    halia::create<ExecutionFeedback>(L, execute(exe, command));
    return 1;
}
static const luaL_Reg functions[] = {
    {"execute", execute_command},
    {"exists_in_path_environment", exists_in_path_environment},
    {"find_in_path_environment", find_in_path_environment},
    {"Child_process", module::process::child_process_ctor},
    {"run", run},
    {nullptr, nullptr}
};

static void proc_option_field(lua_State* L, const char* name, Process_option value) {
    create<Process_option>(L, value);
    lua_setfield(L, -2, name);
}
static int push_process_option_constants(lua_State* L) {
    lua_newtable(L);
    proc_option_field(L, "silent", Process_option::silent);
    return 1;
}

CoreLibrary library::process{"process", [](lua_State* L) {
    module::process::init_execution_feedback_meta(L);
    module::process::init_child_process_meta(L);
    module::process::init_pid_meta(L);
    module::process::init_args_span_meta(L);
    lua_newtable(L);
    luaL_register(L, nullptr, functions);
    create<ArgsSpan>(L, halia::core::args_span());
    lua_setfield(L, -2, "args");
    //push_process_option_constants(L);
    //lua_setfield(L, -2, "Process_option");
    return 1;
}};
