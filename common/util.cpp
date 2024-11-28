#include "util.h"
#include "userdata_helpers.h"

namespace util {
std::optional<std::string> resolve_path_type(lua_State* L, int i) {
    if (is_type<std::filesystem::path>(L, i)) {
        return std::make_optional(check<std::filesystem::path>(L, i).string());
    } else if (lua_isstring(L, i)) {
        return std::make_optional(luaL_checkstring(L, i));
    } else return std::nullopt;
}
}
