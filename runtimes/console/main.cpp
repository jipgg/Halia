#include <halia/core.hpp>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
using namespace std::string_view_literals;

using zstring = char*;
int main(int argc, zstring* argv) {
    halia::core::Launch_options opts{
        .main_entry_point = "../example/init.luau",
    };
    if (argc > 1) {
        opts.main_entry_point = argv[1];
    }
    return halia::core::bootstrap(opts);
}
