#include "library.hpp"
#include "core.hpp"
#include <chrono>
#include "common/common.hpp"
#include <lualib.h>

static int spawn(lua_State* L) {
    if (not lua_isfunction(L, 1)) {
        luaL_argerrorL(L, 1, "not a function");
        return 0;
    }
    Cothread _co{L};
    lua_State* TL = _co.state();
    halia::core::emplace_cothread(std::move(_co));
    Cothread* co = halia::core::find_cothread(TL);
    const int nargs = lua_gettop(L);
    lua_xmove(L, TL, nargs);
    printerr("here");
    int status = lua_resume(TL, L, nargs - 1);
    printerr("there");
    return 0;
}
static int wait(lua_State* L) {
    const double duration = luaL_checknumber(L, 1);
    lua_yield(L, 0);
    auto* co = halia::core::find_cothread(L);
    if (co == nullptr) {
        luaL_errorL(L, "didnt find cothread");
    }
    co->status = Cothread::Status::waiting;
    co->yield_start = Cothread::Steady_clock::now();
    co->yield_duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::duration<double>(duration));
    return 0;
}

static const luaL_Reg table[] = {
    {"spawn", spawn},
    {"wait", wait},
    {nullptr, nullptr}
};

namespace library {
Builtin_library task{"task", [](lua_State* L) -> int {
    lua_newtable(L);
    luaL_register(L, nullptr, table);
    return 1;
}};
}
