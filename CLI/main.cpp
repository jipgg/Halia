#include <common/util.hpp>
#include <boost/process.hpp>
#include <common.hpp>
#include <format>
#include <ranges>
#include <filesystem>
using namespace std::string_view_literals;
namespace bp = boost::process;
namespace fs = std::filesystem;
namespace srv = std::ranges::views;

using zstring = char*;
int main(int argc, zstring* argv) {
    #ifndef VISUAL_STUDIO_DBG
    const zstring* begin = argv + 1;
    const zstring* end = argv + argc;
    std::vector<std::string> args{begin, end};
    /*
    std::string all_args{argv[0]};
    for (int i{1}; i < argc; ++i) {
        all_args.push_back(' ');
        all_args.append(argv[i]);
    }
    */
    #else
    std::vector<std::string> args{};
    args.push_back("main.luau");
    #endif
    /*
    std::string args;
    constexpr std::string_view source_param = "--source=";
    if (auto start = all_args.find(source_param); start != args.npos) {
        std::string_view sources{all_args};
        auto end = sources.find_first_of(' ', start);
        sources = sources.substr(start, end);
        args.append(source_param);
        for (auto e : srv::split(sources, ',')) {
            fs::path path{e.begin(), e.end()};
            if (not fs::exists(path))  {
                printerr(std::format("source file '{}' does not exist.", path.string()));
                return -1;
            }
            args.append(path.string() + ',');
        } 
        args.back() = ' ';
    }
    */
    fs::path exe_path = util::get_executable_path() / "Halia.runtime.console.exe";
    assert(argc > 1);
    bp::system(exe_path.string(), bp::args = args);
    return 0;
}
