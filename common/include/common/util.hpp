#pragma once
#include <filesystem>
#include <lualib.h>
struct lua_State;
namespace util {
std::filesystem::path get_executable_path();
}
