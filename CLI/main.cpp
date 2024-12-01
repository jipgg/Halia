#include <common/util.hpp>
#include <boost/process.hpp>
using namespace std::string_view_literals;
#include <filesystem>
namespace bp = boost::process;
namespace fs = std::filesystem;

using zstring = char*;
int main(int argc, zstring* argv) {
    fs::path exe_path = util::get_executable_path() / "Halia.runtime.console.exe";
    assert(argc > 1);
    bp::system(exe_path.string(), argv[1]);
    return 0;
}
