#pragma once
#include <filesystem>
struct lua_State;
namespace fs = std::filesystem;
using DirectoryEntry = fs::directory_entry;
using Path = fs::path;

namespace exported {
void init_path_meta(lua_State* L);
int path_ctor(lua_State* L);
void init_directory_entry_meta(lua_State* L);
}
