#pragma once
#include <filesystem>
#include <lualib.h>
#include "builtin.h"
#include "userdata_helpers.hpp"
struct lua_State;
namespace util {
std::optional<std::string> resolve_path_type(lua_State* L, int i);
std::filesystem::path get_executable_path();
using Unique_event = std::unique_ptr<builtin::Event>; 
inline void register_event(lua_State* L, Unique_event& ev, const char* fieldname) {
    ev = std::make_unique<builtin::Event>(L);
    push(L, *ev);
    lua_setfield(L, -2, fieldname);
}
}
