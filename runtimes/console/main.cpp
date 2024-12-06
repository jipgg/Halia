#include <halia/core.hpp>
#include <halia/co_tasks.hpp>
#include "common.hpp"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
using namespace std::string_view_literals;
using namespace halia;

using zstring = char*;
int main(int argc, zstring* argv) {
    std::vector<std::string_view> args{argv + 1, argv + argc};
    core::LaunchOptions opts{
        .main_entry_point = "main.luau",
        .args = args,
    };
    constexpr int loading_error_exit_code = -1;
    constexpr int runtime_error_exit_code = -2;
    if (auto error = init(opts)) {
        printerr(*error);
        return loading_error_exit_code;
    }
    core::State state = core::state();
    core::CoThread thread = core::main_thread();
    int status = lua_resume(thread, state, 0);
    while (status == LUA_YIELD or not co_tasks::all_done()) {
        if (auto error = co_tasks::schedule(state)) {
            printerr(std::format("runtime error: {}", error->formatted()));
            return runtime_error_exit_code;
        }
        status = lua_status(thread);
    }
    if (status != LUA_OK) {
        const char* error_message = lua_tostring(thread, -1);
        error_message = error_message ? error_message : "unknown error";

        printerr(std::format("runtime error: {}.", error_message));
        lua_pop(thread, 1);
        return runtime_error_exit_code;
    }
    return 0;
}
