#pragma once
#include "halia_api.hpp"
#include <typeinfo>
#include <unordered_map>
#include <cassert>
#include <lualib.h>
#include <string>

namespace halia {
namespace intern {
extern HALIA_API int unique_tag_incr;
extern HALIA_API std::unordered_map<std::string, int> type_registry;//should still make this thread safe
}
template <class Ty>
int type_tag() {
    constexpr int nulltag = -1;
    static int tag = nulltag;
    if (tag == nulltag) {
        const std::string type_name = typeid(Ty).name();
        auto found_it = intern::type_registry.find(type_name);
        if (found_it == intern::type_registry.end()) {
            intern::type_registry.insert(std::make_pair(type_name, intern::unique_tag_incr++));
        }
        tag = intern::type_registry.at(type_name);
    }
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
    static std::string name{ti.name() + std::string("_meta")};
    return name.c_str();
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
}
