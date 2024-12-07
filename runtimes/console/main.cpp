#include <halia/core.hpp>
#include <halia/co_tasks.hpp>
#include "common.hpp"
#include <halia/ProjectConfiguration.hpp>
#include <Luau/Compiler.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
using namespace std::string_view_literals;
using namespace halia;

using zstring = char*;
int main(int argc, zstring* argv) {
    std::vector<std::string_view> args{argv + 1, argv + argc};
    constexpr int loading_error_exit_code = -1;
    constexpr int runtime_error_exit_code = -2;
    auto result = parse_project_configuration_file("./tests/tests.project.luau");
    if (ErrorInfo* error = std::get_if<ErrorInfo>(&result)) {
        printerr(*error);
        return loading_error_exit_code;
    }
    core::LaunchOptions opts{
        .scripts = std::get<ProjectConfiguration>(result).script_paths,
        .args = args,
    };
    print(opts.scripts.size(), "size");
    if (auto error = init(opts)) {
        printerr(*error);
        return loading_error_exit_code;
    }
    auto initial = core::spawn_scripts(std::span(opts.scripts));
    bool error_occurred = bool(initial);
    std::string error_message = initial ? initial->formatted() : "";
    while (not co_tasks::all_done() and not error_occurred) {
        auto err = co_tasks::schedule(core::state());
        if (err) {
            error_occurred = true;
            error_message = err->formatted();
        }
    }
    if (error_occurred) {
        printerr(error_message);
        return runtime_error_exit_code;
    }
    return 0;
}
