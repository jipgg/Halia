#include "library.hpp"
#include "core.hpp"
#include <chrono>
#include "common/common.hpp"
#include <lualib.h>
#include <algorithm>
#include <thread>
using Steady_clock = std::chrono::steady_clock;
namespace hc = halia::core;
using Lua_ref_guard = std::unique_ptr<int, std::function<void(int*)>>; 
struct Waiting_task {
    Lua_ref_guard guard;
    lua_State* state;
    int nargs = 0;
    Steady_clock::time_point started = Steady_clock::now();
    Steady_clock::duration duration;
    friend bool operator==(const Waiting_task& lhs, const Waiting_task& rhs) {
        return lhs.state == rhs.state;
    }
};
std::vector<Waiting_task> waiting;
std::unordered_map<lua_State*, Lua_ref_guard> fixed_refs;
static Steady_clock::duration steady_clock_duration(double in_sec) {
    return std::chrono::duration_cast<Steady_clock::duration>(std::chrono::duration<double>(in_sec));
}
Lua_ref_guard lock_ref(lua_State* L, int index = -1) {
    auto deleter = [L](int* e) -> void {
        lua_unref(L, *e);
        delete e;
    };
    return Lua_ref_guard{new int(lua_ref(L, index)), deleter};
}
struct Lua_thread_info {
    lua_State* thread;
    int nargs;
    Lua_ref_guard ref;
};
[[nodiscard]] static Error_message_or<Lua_thread_info> create_thread(lua_State* L) {
    if (!lua_isfunction(L, 1)) {
        return "requires a function";
    }
    lua_State* new_thread = lua_newthread(L);
    Lua_ref_guard ref = lock_ref(L, -1);
    lua_pushvalue(L, 1);                      // Push the function to the new thread
    lua_xmove(L, new_thread, 1);              // Move the function to the new thread

    int nargs = lua_gettop(L) - 1;            // Get additional arguments
    lua_xmove(L, new_thread, nargs);          // Move arguments to the new thread
    lua_pop(L, 1);
    luaL_sandboxthread(new_thread);
    return Lua_thread_info{.thread = new_thread, .nargs = nargs, .ref = std::move(ref)};
}

namespace library {
int co_spawn(lua_State* L) {
    auto result = create_thread(L);
    if (std::string* error_message = std::get_if<std::string>(&result)) {
        luaL_argerrorL(L, 1, error_message->c_str());
        return 0;
    }
    auto& [new_thread, nargs, ref] = std::get<Lua_thread_info>(result);
    lua_resume(new_thread, hc::lua_state(), nargs);
    lua_getref(L, *ref);
    fixed_refs.emplace(new_thread, std::move(ref));
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
        fixed_refs.erase(task->state);
    }
    if (all_tasks_done()) {//scuffed fix
        printerr("clearing tasks");
        fixed_refs.clear();
    }
    return std::nullopt;
}
}

