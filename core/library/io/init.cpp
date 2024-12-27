#include "library.hpp"
#include "common/constext.hpp"
#include "type_utils.hpp"
#include <string_view>
enum class Attribute {
    reset_all,
    bold,
    dim,
    italic,
    underline,
    blink,
    reverse,
    hidden,
    strikethrough,
    reset_bold_dim,
    reset_italic,
    reset_underline,
    reset_blink,
    reset_reverse,
    reset_hidden,
    reset_strikethrough
};
static constexpr const char* to_escape_sequence(Attribute attr) {
    switch (attr) {
        case Attribute::reset_all: return "\033[0m";
        case Attribute::bold: return "\033[1m";
        case Attribute::dim: return "\033[2m";
        case Attribute::italic: return "\033[3m";
        case Attribute::underline: return "\033[4m";
        case Attribute::blink: return "\033[5m";
        case Attribute::reverse: return "\033[7m";
        case Attribute::hidden: return "\033[8m";
        case Attribute::strikethrough: return "\033[9m";
        case Attribute::reset_bold_dim: return "\033[22m";
        case Attribute::reset_italic: return "\033[23m";
        case Attribute::reset_underline: return "\033[24m";
        case Attribute::reset_blink: return "\033[25m";
        case Attribute::reset_reverse: return "\033[27m";
        case Attribute::reset_hidden: return "\033[28m";
        case Attribute::reset_strikethrough: return "\033[29m";
    }
}
constexpr auto attribute_count = static_cast<int>(Attribute::reset_strikethrough) + 1;
enum class Color {
    black,
    red,
    green,
    yellow,
    blue,
    magenta,
    cyan,
    white,
    bright_black,
    bright_red,
    bright_green,
    bright_yellow,
    bright_blue,
    bright_magenta,
    bright_cyan,
    bright_white
};
static constexpr const char* to_foreground_color(Color col) {
    switch (col) {
        case Color::black: return "\033[30m";
        case Color::red: return "\033[31m";
        case Color::green: return "\033[32m";
        case Color::yellow: return "\033[33m";
        case Color::blue: return "\033[34m";
        case Color::magenta: return "\033[35m";
        case Color::cyan: return "\033[36m";
        case Color::white: return "\033[37m";
        case Color::bright_black: return "\033[90m";
        case Color::bright_red: return "\033[91m";
        case Color::bright_green: return "\033[92m";
        case Color::bright_yellow: return "\033[93m";
        case Color::bright_blue: return "\033[94m";
        case Color::bright_magenta: return "\033[95m";
        case Color::bright_cyan: return "\033[96m";
        case Color::bright_white: return "\033[97m";
    }
}
static constexpr const char* to_background_color(Color col) {
    switch (col) {
        case Color::black: return "\033[40m";
        case Color::red: return "\033[41m";
        case Color::green: return "\033[42m";
        case Color::yellow: return "\033[43m";
        case Color::blue: return "\033[44m";
        case Color::magenta: return "\033[45m";
        case Color::cyan: return "\033[46m";
        case Color::white: return "\033[47m";
        case Color::bright_black: return "\033[100m";
        case Color::bright_red: return "\033[101m";
        case Color::bright_green: return "\033[102m";
        case Color::bright_yellow: return "\033[103m";
        case Color::bright_blue: return "\033[104m";
        case Color::bright_magenta: return "\033[105m";
        case Color::bright_cyan: return "\033[106m";
        case Color::bright_white: return "\033[107m";
    }
}
constexpr auto color_count = static_cast<int>(Color::bright_white) + 1;
template<constext::Enum T, int N>
void push_enum_table(lua_State* L) {
    constexpr auto arr = constext::to_array<T, N>();
    lua_newtable(L);
    for (const constext::EnumInfo& ei : arr) {
        halia::create<constext::EnumInfo>(L, ei);
        lua_setfield(L, -2, std::string(ei.name).c_str());
    }
}
static int set_attribute(lua_State* L) {
    const std::string_view key = luaL_checkstring(L, 1);
    return 0;
}
void register_enums(lua_State* L) {
    lua_newtable(L);
}
namespace library {
CoreLibrary io("io", [](lua_State* L) -> int {
    lua_newtable(L);
    push_enum_table<Color, color_count>(L);
    lua_setfield(L, -2, "Color");
    push_enum_table<Attribute, attribute_count>(L);
    lua_setfield(L, -2, "Attribute");
    return 1;
});
}
