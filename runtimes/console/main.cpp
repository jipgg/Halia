#include <halia/core.hpp>
#include <halia/co_tasks.hpp>
#include "common.hpp"
#include "common/util.hpp"
#include <halia/ProjectConfiguration.hpp>
#include <Luau/Compiler.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <optional>
#endif
using namespace std::string_view_literals;
namespace fs = std::filesystem;
using namespace halia;
using zstring = char*;

static std::optional<size_t> find_param_index(std::string_view param, std::span<std::string_view> argv) {
    for(size_t i{}; i < argv.size(); ++i) {
        auto v = argv[i];
        if (v.size() < param.size()) continue; 
        if (v.substr(0, param.size()) == param) {
            return i;
        }
    }
    return std::nullopt;
} 
int main(int argc, zstring* argv) {
    std::vector<std::string_view> args{argv + 1, argv + argc};
    constexpr int loading_error_exit_code = -1;
    constexpr int runtime_error_exit_code = -2;
    fs::path config_path;
    constexpr std::string_view project_keyw = "--project=";
    if (auto found = find_param_index(project_keyw, args)) {
        config_path = args[*found].substr(project_keyw.size());
    } else {
        for (auto entry : fs::directory_iterator(fs::current_path())) {
            auto path = entry.path();
            if (path.string().find(".project.luau") != std::string::npos) {
                config_path = path;
                break;
            }
        }
        ErrorInfo err{"Couldn't find project file in working directory."};
        printerr(err);
        return loading_error_exit_code;
    }
    auto parsed_config = parse_project_configuration_file(config_path);
    if (ErrorInfo* err = std::get_if<ErrorInfo>(&parsed_config)) {
        printerr(*err);
        return loading_error_exit_code;
    }
    core::LaunchOptions opts{
        .scripts = std::get<ProjectConfiguration>(parsed_config).script_paths,
        .args = args,
    };
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
