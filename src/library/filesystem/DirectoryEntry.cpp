#include "init.h"
#include <lualib.h>
#include "lua_atom.h"
#include "userdata_helpers.hpp"
#include "metamethod.h"
constexpr auto type = "waw.filesystem.DirectoryEntry";

static int namecall(lua_State* L) {
    auto& r = check<fs::directory_entry>(L, 1);
    int atom{};
    lua_namecallatom(L, &atom);
    using la = lua_atom;
    switch (static_cast<la>(atom)) {
        case la::is_directory:
            lua_pushboolean(L, r.is_directory());
            return 1;
        case la::is_fifo:
            lua_pushboolean(L, r.is_fifo());
            return 1;
        case la::file_path:
            create<fs::path>(L, r.path());
            return 1;
        case la::is_socket:
            lua_pushboolean(L, r.is_socket());
            return 1;
        case la::is_other:
            lua_pushboolean(L, r.is_other());
            return 1;
        case la::is_symlink:
            lua_pushboolean(L, r.is_symlink());
            return 1;
        case la::is_block_file:
            lua_pushboolean(L, r.is_block_file());
            return 1;
        case la::is_regular_file:
            lua_pushboolean(L, r.is_regular_file());
            return 1;
        case la::is_character_file:
            lua_pushboolean(L, r.is_character_file());
            return 1;
        default: return 0;
    }
    return 0;
}
const luaL_Reg metatable[] = {
    {metamethod::namecall, namecall},
    {nullptr, nullptr}
};
namespace exported {
void init_directory_entry_meta(lua_State *L) {
    if (luaL_newmetatable(L, metatable_name<DirectoryEntry>())) {
        luaL_register(L, nullptr, metatable);
        lua_pushstring(L, type);
        lua_setfield(L, -2, metamethod::type);
    }
    lua_pop(L, 1);
}
}
