#include "common.h"
namespace fs = std::filesystem;
#if defined(_WIN32)
    #include <Windows.h>
#elif defined(__linux__)
    #include <unistd.h>
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
#endif

namespace util {
fs::path get_executable_path() {
#if defined(_WIN32)
    char buffer[MAX_PATH];
    GetModuleFileName(nullptr, buffer, MAX_PATH);
    return fs::path(buffer).parent_path();
#elif defined(__linux__)
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        return fs::path(buffer).parent_path();
    }
    return "";
#elif defined(__APPLE__)
    char buffer[PATH_MAX];
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) == 0) {
        return fs::path(buffer).parent_path();
    }
    return "";
#else
    return std::filesystem::current_path(); //fallback
#endif
}
}
