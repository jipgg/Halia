#define VISUAL_STUDIO_DBG
#include <common/util.hpp>
#include <boost/process.hpp>
#include <span>
using namespace std::string_view_literals;
#include <filesystem>
namespace bp = boost::process;
namespace fs = std::filesystem;

using zstring = char*;
int main(int argc, zstring* argv) {
    #ifndef VISUAL_STUDIO_DBG
    const zstring* begin = argv + 1;
    const zstring* end = argv + argc;
    std::vector<std::string> args(begin, end);
    #else
    std::vector<std::string> args{};
    args.push_back("main.luau");
    #endif
    fs::path exe_path = util::get_executable_path() / "Halia.runtime.console.exe";
    assert(argc > 1);
    bp::system(exe_path.string(), bp::args = args);
    return 0;
}
