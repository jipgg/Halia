#include "module.hpp"
#include <lualib.h>
#include "common/Namecall_atom.hpp"
#include "type_utils.hpp"
#include "common/metamethod.hpp"
static constexpr auto type = "Path";
using namespace halia;

static int div(lua_State* L) {
    Path lhs;
    Path rhs;
    if (is_type<Path>(L, 1)) {
        lhs = check<Path>(L, 1);
    } else if (lua_isstring(L, 1)) {
        lhs = luaL_checkstring(L, 1);
    } else {
        lua_pushstring(L, "invalid lhs type");
        lua_error(L);
        return 0;
    }
    if (is_type<Path>(L, 2)) {
        rhs = check<Path>(L, 2);
    } else if (lua_isstring(L, 2)) {
        rhs = luaL_checkstring(L, 2);
    } else {
        lua_pushstring(L, "invalid rhs type");
        lua_error(L);
        return 0;
    }
    create<Path>(L, lhs / rhs);
    return 1;
}
static int namecall(lua_State* L) {
    int atom{};
    lua_namecallatom(L, &atom);
    auto& r = check<Path>(L, 1);
    using A = NamecallAtom;
    switch (static_cast<A>(atom)) {
        case A::stem:
            create<Path>(L, r.stem());
            return 1;
        case A::is_empty:
            lua_pushboolean(L, r.empty());
            return 1;
        case A::file_name:
            create<Path>(L, r.filename());
            return 1;
        case A::has_stem:
            lua_pushboolean(L, r.has_stem());
            return 1;
        case A::root_path:
            create<Path>(L, r.root_path());
            return 1;
        case A::parent_path:
            create<Path>(L, r.parent_path());
            return 1;
        case A::is_absolute:
            lua_pushboolean(L, r.is_absolute());
            return 1;
        case A::is_relative:
            lua_pushboolean(L, r.is_relative());
            return 1;
        case A::extension:
            create<Path>(L, r.extension());
            return 1;
        case A::has_extenson:
            lua_pushboolean(L, r.has_extension());
            return 1;
        case A::replace_extension:
            r.replace_extension(luaL_checkstring(L, 2));
            return 0;
        case A::relative_path:
            create<Path>(L, r.relative_path());
            return 1;
        case A::has_relative_path:
            lua_pushboolean(L, r.has_relative_path());
            return 1;
        case A::compare:
            lua_pushinteger(L, r.compare(check<Path>(L, 2)));
            return 1;
        case A::root_name:
            create<Path>(L, r.root_name());
            return 1;
        case A::root_directory:
            create<Path>(L, r.root_directory());
            return 1;
        case A::has_root_path:
            lua_pushboolean(L, r.has_root_path());
            return 1;
        case A::has_root_name:
            lua_pushboolean(L, r.has_root_name());
            return 1;
        case A::has_root_directory:
            lua_pushboolean(L, r.has_root_directory());
            return 1;
        default:
            return 0;
    }
}
static int tostring(lua_State* L) {
    auto& r = check<Path>(L, 1);
    lua_pushstring(L, r.string().c_str());
    return 1;
}
static const luaL_Reg path_metatable[] = {
    {metamethod::tostring, tostring},
    {metamethod::div, div},
    {metamethod::namecall, namecall},
    {nullptr, nullptr}
};

void module::filesystem::init_path_meta(lua_State* L) {
    if (luaL_newmetatable(L, metatable_name<Path>())) {
        luaL_register(L, nullptr, path_metatable);
        lua_pushstring(L, type);
        lua_setfield(L, -2, metamethod::type);
    }
    lua_pop(L, 1);
}
int module::filesystem::path_ctor(lua_State* L) {
    create<Path>(L, luaL_checkstring(L, 1));
    return 1;
}
