#include "core.hpp"
#include <chrono>
#include "common.hpp"
#include <variant>
#include "co_tasks.hpp"
#include <lualib.h>
#include <algorithm>
using SteadyClock = std::chrono::steady_clock;
namespace hc = halia::core;
using halia::ErrorInfo;
using LuaRefGuard = std::unique_ptr<int, std::function<void(int*)>>; 
struct CoTask {
    LuaRefGuard guard;
    lua_State* state;
    int nargs = 0;
    SteadyClock::time_point started = SteadyClock::now();
    SteadyClock::duration duration;
    friend bool operator==(const CoTask& lhs, const CoTask& rhs) {
        return lhs.state == rhs.state;
    }
};
std::vector<CoTask> waiting;
std::unordered_map<lua_State*, LuaRefGuard> fixed_refs;
static SteadyClock::duration steady_clock_duration(double in_sec) {
    return std::chrono::duration_cast<SteadyClock::duration>(std::chrono::duration<double>(in_sec));
}
LuaRefGuard lock_ref(lua_State* L, int index = -1) {
    auto deleter = [L](int* e) -> void {
        lua_unref(L, *e);
        delete e;
    };
    return LuaRefGuard{new int(lua_ref(L, index)), deleter};
}
struct LuaThreadInfo {
    lua_State* thread;
    int nargs;
    LuaRefGuard ref;
};
[[nodiscard]] static std::variant<LuaThreadInfo, halia::ErrorInfo> create_thread(lua_State* L) {
    if (!lua_isfunction(L, 1)) {
        return halia::ErrorInfo("requires a function");
    }
    lua_State* new_thread = lua_newthread(L);
    LuaRefGuard ref = lock_ref(L, -1);
    lua_pushvalue(L, 1);                      
    lua_xmove(L, new_thread, 1);

    int nargs = lua_gettop(L) - 1;            
    lua_xmove(L, new_thread, nargs);          
    lua_pop(L, 1);
    luaL_sandboxthread(new_thread);
    return LuaThreadInfo{.thread = new_thread, .nargs = nargs, .ref = std::move(ref)};
}

namespace library {
int co_spawn(lua_State* L) {
    auto result = create_thread(L);
    if (auto* error_info = std::get_if<ErrorInfo>(&result)) {
        luaL_argerrorL(L, 1, error_info->formatted().c_str());
        return 0;
    }
    auto& [new_thread, nargs, ref] = std::get<LuaThreadInfo>(result);
    lua_resume(new_thread, hc::state(), nargs);
    lua_getref(L, *ref);
    fixed_refs.emplace(new_thread, std::move(ref));
    return 1;
}
int co_wait(lua_State* L) {
    double duration = luaL_checknumber(L, 1);
    auto predicate = [&L](CoTask& e) {return e.state == L;};
    auto it = std::find_if(waiting.begin(), waiting.end(), predicate);
    if (it == waiting.end()) {
        waiting.emplace_back(CoTask{
            .state = L,
            .nargs = 0,
            .duration = steady_clock_duration(duration),
        });
    } else {
        it->duration += steady_clock_duration(duration);
    }
    return lua_yield(L, 0);
}
}
namespace halia::co_tasks {
void add_task(lua_State* thread, int index) {
    LuaRefGuard guard{lock_ref(thread, -1)};
    waiting.emplace_back(CoTask{
        .guard = std::move(guard),
        .state = thread,
        .started = SteadyClock::now(),
        .duration = SteadyClock::duration::zero(),
    });
}
bool all_done() noexcept {
    return waiting.empty();
}
std::optional<ErrorInfo> schedule(lua_State* L) {
    auto now = SteadyClock::now();
    auto completed = std::vector<CoTask*>();

    for (CoTask& task : waiting) {
        const auto duration = now - task.started;
        if (duration >= task.duration) {
            int status = lua_status(task.state);
            if (status == LUA_YIELD) status = lua_resume(task.state, L, task.nargs);
            if (status == LUA_OK) {
                completed.push_back(&task);
            } else if (status != LUA_YIELD) {
                return ErrorInfo(lua_tostring(task.state, -1));
            }
        }
    }
    for (CoTask* task : completed) {
        waiting.erase(std::remove(waiting.begin(), waiting.end(), *task), waiting.end());
        fixed_refs.erase(task->state);
    }
    if (all_done()) {//scuffed fix
        fixed_refs.clear();
    }
    return std::nullopt;
}
}

