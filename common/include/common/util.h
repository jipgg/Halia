#pragma once
#include <filesystem>
#include <lualib.h>
struct lua_State;
namespace util {
std::optional<std::string> resolve_path_type(lua_State* L, int i);
std::filesystem::path get_executable_path();
}
