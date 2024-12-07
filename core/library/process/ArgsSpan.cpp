#include "module.hpp"
#include "type_utils.hpp"
#include "common/NamecallAtom.hpp"
#include "common/metamethod.hpp"
#include "common.hpp"
constexpr const char* type = "ArgsSpan";
constexpr std::string_view size_key = "size";

static bool is_param(std::string_view param) {
    return param.substr(0, 2) == "--";
}

static int namecall(lua_State* L) {
    auto& self = check<ArgsSpan>(L, 1);
    int atom;
    lua_namecallatom(L, &atom);
    using Na = NamecallAtom;
    switch (static_cast<Na>(atom)) {
        case Na::at: {
            const int idx = luaL_checkinteger(L, 2);
            if (idx >= self.size()) {
                luaL_argerrorL(L, 2, "index is out of range."); 
                return 0;
            }
            lua_pushlstring(L, self[idx].data(), self[idx].size());
            return 1;
        }
        case Na::to_table: {
            lua_newtable(L);
            int i{1};
            for (const std::string_view& arg : self) {
                lua_pushlstring(L, arg.data(), arg.size());
                lua_rawseti(L, -2, i++);
            }
            return 1;
        }
        case Na::parse_params: {
            int i{};
            lua_newtable(L);
            while(i < self.size()) {
                auto v = self[i];
                if (not is_param(v)) {
                    ++i;
                    continue;
                }
                const int inext = i + 1;
                if (inext >= self.size() or is_param(self[inext])) {
                    ++i;
                    continue;
                }
                const std::string_view vnext = self[inext];
                lua_pushlstring(L, vnext.data(), vnext.size());
                lua_setfield(L, -2, std::string(v.substr(2)).c_str());
                i += 2;
            }
            return 1;
        }
        case Na::parse_flags: {
            lua_newtable(L);
            for (auto& arg : self) {
                if (arg[0] != '-') continue;
                if (arg[1] == '-') continue;
                lua_pushboolean(L, true);
                lua_setfield(L, -2, std::string(arg.substr(1)).c_str());
            }
            return 1;
        }
        case Na::for_each: {
            if (not lua_isfunction(L, 2)) {
                luaL_argerrorL(L, 2, "not a function");
                return 0;
            }
            const int fn = lua_ref(L, 2);
            ScopeGuard unref([&fn, &L] {lua_unref(L, fn);});
            for (std::string_view& v : self) {
                lua_getref(L, fn);
                lua_pushlstring(L, v.data(), v.size());
                if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
                    lua_error(L);
                    return 0;
                }
            }
            return 0;
        }
        default:
            luaL_errorL(L, "invalid namecall");
    }
    return 0;
}
static int index(lua_State* L) {
    auto& self = check<ArgsSpan>(L, 1);
    const std::string_view key = luaL_checkstring(L, 2);
    switch (key[0]) {
        case size_key[0]:
            if (key == size_key) {
                lua_pushinteger(L, self.size());
                return 1;
            }
    }
    luaL_errorL(L, "invalid index");
    return 0;
}
static int newindex(lua_State* L) {
    luaL_errorL(L, "is readonly");
    return 0;
}
static const luaL_Reg meta[] = {
    {metamethod::namecall, namecall},
    {metamethod::index, index},
    {metamethod::newindex, newindex},
    {nullptr, nullptr}
};

void module::process::init_args_span_meta(lua_State* L) {
    if (luaL_newmetatable(L, metatable_name<ArgsSpan>())) {
        luaL_register(L, nullptr, meta);
        lua_pushstring(L, type);
        lua_setfield(L, -2, metamethod::type);
    }
    lua_pop(L, 1);
}
