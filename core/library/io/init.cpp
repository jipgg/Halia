#include "library.hpp"
#include "common/constext.hpp"
constexpr const char* text_color_key = "text_color";

static int index(lua_State* L) {
    return 1;
}

namespace library {
Builtin_library io("io", [](lua_State* L) -> int {
    return 1;
});
}
