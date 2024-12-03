#pragma once
#include <lualib.h>
#include "Builtin_library.hpp"
#include "common/common.hpp"

namespace library {
extern Builtin_library filesystem;
extern Builtin_library math;
extern Builtin_library process;
int co_spawn(lua_State* L);
int co_wait(lua_State* L);
}

namespace co_tasks {
bool all_tasks_done();
Error_message_on_failure schedule_tasks(lua_State* L);
void add_task(lua_State* thread);
}
