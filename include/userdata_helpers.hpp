#pragma once
#include "runtime.h"
#include "builtin.h"
#include <lualib.h>
#include <cassert>
#include <luaconf.h>
namespace intern {
inline int unique_tag_incr{0};
}
template <class Ty>
int type_tag() {
    static const int tag = intern::unique_tag_incr++; 
    return tag;
}
template <class Ty>
bool is_type(lua_State* L, int idx) {
    const bool is_full_userdata = lua_isuserdata(L, idx) and lua_userdatatag(L, idx) == type_tag<Ty>();
    const bool is_light_userdata = lua_islightuserdata(L, idx) and lua_lightuserdatatag(L, idx) == type_tag<Ty>(); 
    return is_full_userdata or is_light_userdata;
}
template<class Ty>
const char* metatable_name() {
    const std::type_info& ti = typeid(Ty);
    return ti.raw_name();
}
template <class Ty, class ...Init_args>
Ty& create(lua_State* L, Init_args&&...args) {
    void* ud = lua_newuserdatatagged(L, sizeof(Ty), type_tag<Ty>());
    new (ud) Ty{std::forward<Init_args>(args)...};
    lua_setuserdatadtor(L, type_tag<Ty>(), [](lua_State* L, void* data) {
        static_cast<Ty*>(data)->~Ty();//cause using placement new, no implicit destruction
    });
    if (luaL_getmetatable(L, metatable_name<Ty>())) {
        lua_setmetatable(L, -2);
    }
    return *static_cast<Ty*>(ud);
}
template <class Ty>
Ty& create_raw(lua_State* L) {
    void* ud = lua_newuserdatatagged(L, sizeof(Ty), type_tag<Ty>());
    lua_setuserdatadtor(L, type_tag<Ty>(), [](lua_State* L, void* data) {
        static_cast<Ty*>(data)->~Ty();//cause using placement new, no implicit destruction
    });
    if (luaL_getmetatable(L, metatable_name<Ty>())) {
        lua_setmetatable(L, -2);
    }
    return *static_cast<Ty*>(ud);
}
template <class Ty>
Ty& check(lua_State* L, int objindex) {
    assert(is_type<Ty>(L, objindex));
    void* ud;
    if (lua_islightuserdata(L, objindex)) {
        ud = lua_tolightuserdatatagged(L, objindex, type_tag<Ty>());
    } else {
        ud = lua_touserdatatagged(L, objindex, type_tag<Ty>());
    }
    return *static_cast<Ty*>(ud);
}
template <class Ty>
void push(lua_State* L, Ty& userdata) {
    lua_pushlightuserdatatagged(L, &userdata, type_tag<Ty>());
    if (luaL_getmetatable(L, metatable_name<Ty>())) {
        lua_setmetatable(L, -2);
    }
}
