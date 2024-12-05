#pragma once
#include <filesystem>
#include "type_utils.hpp"
struct lua_State;
namespace fs = std::filesystem;
using Directory_entry = fs::directory_entry;
using File_path = fs::path;
inline std::optional<std::string> resolve_path_type(lua_State* L, int i) {
    if (halia::is_type<std::filesystem::path>(L, i)) {
        return std::make_optional(halia::check<std::filesystem::path>(L, i).string());
    } else if (lua_isstring(L, i)) {
        return std::make_optional(luaL_checkstring(L, i));
    } else return std::nullopt;
}

namespace module::filesystem {
void init_path_meta(lua_State* L);
int path_ctor(lua_State* L);
void init_directory_entry_meta(lua_State* L);
}
