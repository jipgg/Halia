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
};
[[nodiscard]] static Error_message_or<Lua_thread_info> init_thread(lua_State* L) {
    if (!lua_isfunction(L, 1)) {
        return "requires a function";
    }
    lua_State* new_thread = lua_newthread(halia::core::lua_state()); // Create new coroutine
    lua_pushvalue(L, 1);                      // Push the function to the new thread
    lua_xmove(L, new_thread, 1);              // Move the function to the new thread

    int nargs = lua_gettop(L) - 1;            // Get additional arguments
    lua_xmove(L, new_thread, nargs);          // Move arguments to the new thread
    return Lua_thread_info{.thread = new_thread, .nargs = nargs};
}

static int spawn(lua_State* L) {
    auto result = init_thread(L);
    if (std::string* error_message = std::get_if<std::string>(&result)) {
        luaL_argerrorL(L, 1, error_message->c_str());
        return 0;
    }
    auto& [new_thread, nargs] = std::get<Lua_thread_info>(result);
    lua_resume(new_thread, halia::core::lua_state(), nargs);
    return 0; // Return to main Lua state
}
static int delay(lua_State* L) {
    const double amount = luaL_checknumber(L, 1);
    lua_remove(L, 1);
    auto result = init_thread(L);
    if (std::string* error_message = std::get_if<std::string>(&result)) {
        luaL_argerrorL(L, 2, error_message->c_str());
        return 0;
    }
    auto& [new_thread, nargs] = std::get<Lua_thread_info>(result);
    waiting.emplace_back(Waiting_task{
        .state = new_thread,
        .nargs = nargs,
        .duration = steady_clock_duration(amount),
    });
    return 0; // Return to main Lua state
}
static int wait(lua_State* L) {
    namespace sc = std::chrono;
    double duration = luaL_checknumber(L, 1);
    waiting.emplace_back(Waiting_task{
        .state = L,
        .duration = sc::duration_cast<Steady_clock::duration>(sc::duration<double>(duration)),
    });
    return lua_yield(L, 0); // Yield the coroutine
}

static const luaL_Reg table[] = {
    {"spawn", spawn},
    {"wait", wait},
    {"delay", delay},
    {nullptr, nullptr}
};

namespace library {
Builtin_library task{"co_task", [](lua_State* L) -> int {
    lua_newtable(L);
    luaL_register(L, nullptr, table);
    return 1;
}};
namespace task_exports {
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
            print(duration, task.duration);
            printerr("waited");
            int status = lua_status(L);
            status = lua_resume(task.state, L, task.nargs);
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
