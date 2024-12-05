#pragma once
#include "halia_core_api.hpp"
#include "Error_info.hpp"
#include <optional>
struct lua_State;

namespace halia {
namespace co_tasks {
HALIA_CORE_API bool all_done() noexcept;
HALIA_CORE_API std::optional<Error_info> schedule(lua_State* L);
HALIA_CORE_API void add_task(lua_State* thread);
}
}
