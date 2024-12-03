#include "library.hpp"
#include "core.hpp"
#include <chrono>
#include "common/common.hpp"
#include <lualib.h>
#include <algorithm>
#include <thread>
using Steady_clock = std::chrono::steady_clock;
namespace hc = halia::core;
struct Waiting_task {
    lua_State* state;
    int nargs = 0;
    Steady_clock::time_point started = Steady_clock::now();
    Steady_clock::duration duration;
    friend bool operator==(const Waiting_task& lhs, const Waiting_task& rhs) {
        return lhs.state == rhs.state;
    }
};
std::vector<Waiting_task> waiting;
static Steady_clock::duration steady_clock_duration(double in_sec) {
    return std::chrono::duration_cast<Steady_clock::duration>(std::chrono::duration<double>(in_sec));
}
struct Lua_thread_info {
    lua_State* thread;
    int nargs;
    int ref;
};
[[nodiscard]] static Error_message_or<Lua_thread_info> init_thread(lua_State* L) {
    if (!lua_isfunction(L, 1)) {
        return "requires a function";
    }
    lua_State* new_thread = lua_newthread(L);
    int ref = lua_ref(L, -1);
    lua_pushvalue(L, 1);                      // Push the function to the new thread
    lua_xmove(L, new_thread, 1);              // Move the function to the new thread

    int nargs = lua_gettop(L) - 1;            // Get additional arguments
    lua_xmove(L, new_thread, nargs);          // Move arguments to the new thread
    lua_pop(L, 1);
    luaL_sandboxthread(new_thread);
    return Lua_thread_info{.thread = new_thread, .nargs = nargs, .ref = ref};
}

namespace library {
int co_spawn(lua_State* L) {
    auto result = init_thread(L);
    if (std::string* error_message = std::get_if<std::string>(&result)) {
        luaL_argerrorL(L, 1, error_message->c_str());
        return 0;
    }
    auto& [new_thread, nargs, ref] = std::get<Lua_thread_info>(result);
    lua_resume(new_thread, hc::lua_state(), nargs);
    lua_getref(L, ref);
    lua_unref(L, ref);
    return 1;
}
int co_wait(lua_State* L) {
    double duration = luaL_checknumber(L, 1);
    auto predicate = [&L](Waiting_task& e) {return e.state == L;};
    auto it = std::find_if(waiting.begin(), waiting.end(), predicate);
    if (it == waiting.end()) {
        waiting.emplace_back(Waiting_task{
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
namespace co_tasks {
void add_task(lua_State* thread) {
    waiting.emplace_back(Waiting_task{
        .state = thread,
        .started = Steady_clock::now(),
        .duration = Steady_clock::duration::zero(),
    });
}
bool all_tasks_done() {
    return waiting.empty();
}
Error_message_on_failure schedule_tasks(lua_State* L) {
    auto now = Steady_clock::now();
    auto completed = std::vector<Waiting_task*>();

    for (Waiting_task& task : waiting) {
        const auto duration = now - task.started;
        if (duration >= task.duration) {
            int status = lua_status(task.state);
            if (status == LUA_YIELD) status = lua_resume(task.state, L, task.nargs);
            if (status == LUA_OK) {
                completed.push_back(&task);
            } else if (status != LUA_YIELD) {
                return lua_tostring(task.state, -1);
            }
        }
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ns);
    }
    for (Waiting_task* task : completed) {
        waiting.erase(std::remove(waiting.begin(), waiting.end(), *task), waiting.end());
    }
    return std::nullopt;
}
}

