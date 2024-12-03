#pragma once
#include <lualib.h>
#include "Builtin_library.hpp"
namespace library {
extern Builtin_library filesystem;
extern Builtin_library math;
extern Builtin_library process;
extern Builtin_library task;
namespace task_exports {
bool all_tasks_done();
void schedule_tasks(lua_State* L);
void add_task(lua_State* thread);
}
}
