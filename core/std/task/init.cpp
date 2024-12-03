#include "library.hpp"
#include "core.hpp"
#include <chrono>
#include "common/common.hpp"
#include <lualib.h>
#include <algorithm>
#include <thread>
using Steady_clock = std::chrono::steady_clock;
struct Waiting_task {
    lua_State* state;
    Steady_clock::time_point started;
    Steady_clock::duration duration;
    friend bool operator==(const Waiting_task& lhs, const Waiting_task& rhs) {
        return lhs.state == rhs.state;
    }
};
std::vector<Waiting_task> waiting;
static bool err = false;

enum class Task_status {Completed, Waiting, Yielding_indefinitely, Error};

static int spawn(lua_State* L) {
    if (!lua_isfunction(L, 1)) {
        luaL_errorL(L, "task.spawn requires a function");
        return 0;
    }
    lua_State* new_thread = lua_newthread(halia::core::lua_state()); // Create new coroutine
    lua_pushvalue(L, 1);                      // Push the function to the new thread
    lua_xmove(L, new_thread, 1);              // Move the function to the new thread

    int nargs = lua_gettop(L) - 1;            // Get additional arguments
    lua_xmove(L, new_thread, nargs);          // Move arguments to the new thread

    if (lua_resume(new_thread, halia::core::lua_state(), nargs) == LUA_YIELD) {
        /*
        waiting.emplace_back(Waiting_task{
            .state = new_thread,
            .started = Steady_clock::now(),
            .duration = Steady_clock::duration::zero(), // No wait by default
        });
        */
    }
    return 0; // Return to main Lua state
}
static int wait(lua_State* L) {
    namespace sc = std::chrono;
    double duration = luaL_checknumber(L, 1);
    waiting.emplace_back(Waiting_task{
        .state = L,
        .started = Steady_clock::now(),
        .duration = sc::duration_cast<Steady_clock::duration>(sc::duration<double>(duration)),
    });
    return lua_yield(L, 0); // Yield the coroutine
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
using Wt_vector = std::vector<Waiting_task*>;
struct Task_params {
    lua_State* main;
    Task_status status;
    Waiting_task& task;
    Wt_vector& completed;
    Wt_vector& to_resume;
};
namespace task_exports {
void add_task(lua_State* thread) {
    waiting.emplace_back(Waiting_task{
        .state = thread,
        .started = Steady_clock::now(),
        .duration = Steady_clock::duration::zero(), // No delay for the main thread
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
            print(duration, task.duration);
            printerr("waited");
            int status = lua_status(L);
            status = lua_resume(task.state, L, 0);
            if (status == LUA_OK) {
                completed.push_back(&task); // Mark completed
            } else if (status != LUA_YIELD) {
                return lua_tostring(task.state, -1);
                //luaL_error(L, lua_tostring(task.state, -1)); // Handle errors
            }
        }
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ns);
    }

    // Clean up completed tasks
    for (Waiting_task* task : completed) {
        waiting.erase(std::remove(waiting.begin(), waiting.end(), *task), waiting.end());
    }
    return std::nullopt;
}
}
}
