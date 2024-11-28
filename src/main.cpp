#include <cassert>
#include "runtime.h"
#include "util.h"
#include "common.h"
#include <lua.h>
#include <lualib.h>
#include <filesystem>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
using namespace std::string_view_literals;

using zstring = char*;
int main(int argc, zstring* argv) {
    #ifdef _WIN32
    enable_ansi_escape_sequences();
    #endif
    runtime::Launch_options opts{
        .main_entry_point = "../example/init.luau",
        .bin_path = util::get_executable_path(),
    };
    if (argc > 1) {
        opts.main_entry_point = argv[1];
    }
    return runtime::bootstrap(opts);
}
