#pragma once
#include "halia_core_api.hpp"
#include <optional>
#include <string>
struct lua_State;

namespace halia {
using Error_message_on_failure = std::optional<std::string>;
namespace co_tasks {
HALIA_CORE_API bool all_done() noexcept;
HALIA_CORE_API Error_message_on_failure schedule(lua_State* L);
HALIA_CORE_API void add_task(lua_State* thread);
}
}
