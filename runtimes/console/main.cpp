#include <halia/core.hpp>
#include "common/common.hpp"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
using namespace std::string_view_literals;

using zstring = char*;
int main(int argc, zstring* argv) {
    std::vector<std::string_view> args{argv + 1, argv + argc};
    halia::core::Launch_options opts{
        .main_entry_point = "main.luau",
        .args = args,
    };
    if (argc > 1) {
        //printerr("AAAAAAAAAAAAAAAAA");
        //opts.main_entry_point = argv[1];
    }
    return halia::core::bootstrap(opts);
}
