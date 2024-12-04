#pragma once
#include <source_location>
#include <string>
#include <type_traits>
#include <string_view>
#include <array>
#include <cassert>
#include <ranges>
#include <format>
#include <iostream>
namespace constext {
template<class Val, Val v>
struct Value {
    static const constexpr auto value_ = v;
    constexpr operator Val() const {return v;}
};
template <class Func, std::size_t... Indices>
constexpr void unroll_for(Func fn, std::index_sequence<Indices...>) {
  (fn(Value<std::size_t, Indices>{}), ...);
}
template <std::size_t count, typename Func>
constexpr void unroll_for(Func fn) {
  unroll_for(fn, std::make_index_sequence<count>());
}
#define constext_COUNT_VA_ARGS(...) constext_COUNT_VA_ARGS_IMPL(__VA_ARGS__, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, \
    49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, \
    21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define constext_COUNT_VA_ARGS_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
    _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, \
    _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, count, ...) count
#define constext_ENUM_SENTINEL_V _end
template <class Ty>
concept Enum = std::is_enum_v<Ty>;
template <Enum Ty>
consteval std::size_t count() {
    return static_cast<std::size_t>(Ty::constext_ENUM_SENTINEL_V);
}
#define constext_SENTINEL_ENUM(ENUM_T, ...) enum class ENUM_T {__VA_ARGS__, constext_ENUM_SENTINE_V}
#define constext_ENUM(ENUM_T, ...) enum class ENUM_T {__VA_ARGS__};\
template <>\
consteval std::size_t constext::count<Enum_type>() {\
    return constext_COUNT_VA_ARGS(__VA_ARGS__);\
}
template <Enum Ty, Ty Last_item>
consteval std::size_t count() {
    return static_cast<std::size_t>(Last_item) + 1;
}
struct Enum_info {
    std::string_view type;
    std::string_view name;
    std::string_view raw;
    int value;
};
template <Enum Ty>
struct Enum_element {
    std::string_view name;
    int value;
    constexpr Ty to_enum() const {return Ty(value);}
    constexpr operator Ty() const {return Ty(value);}
};
namespace detail {
#if defined (_MSC_VER)//msvc compiler
template <Enum Ty, Ty val>
consteval Enum_info enum_info() {
    const std::string_view raw{std::source_location::current().function_name()};
    const std::string enum_t_keyw{"<enum "};
    auto found = raw.find(enum_t_keyw);
    std::string_view enum_t = raw.substr(raw.find(enum_t_keyw) + enum_t_keyw.size());
    bool is_enum_class = enum_t.find("::") != std::string::npos;
    enum_t = enum_t.substr(0, enum_t.find_first_of(','));
    const std::string preceding = enum_t_keyw + std::string(enum_t) + ",";
    std::string_view enum_v = raw.substr(raw.find(preceding) + preceding.size());
    if (enum_v.find("::")) {
        enum_v = enum_v.substr(enum_v.find_last_of("::") + 1);
    }
    enum_v = enum_v.substr(0, enum_v.find_first_of(">"));
    return {
        .type = enum_t,
        .name = enum_v,
        .raw = raw,
        .value = int(val),
    };
}
#elif defined(__clang__) || defined(__GNUC__)
template <Enum Ty, Ty val>
consteval Enum_info enum_info() {
    using sv = std::string_view;
    const sv raw{std::source_location::current().function_name()};
    const sv enum_find{"Ty = "}; 
    const sv value_find{"val = "};
    sv enum_t = raw.substr(raw.find(enum_find) + enum_find.size());
    enum_t = enum_t.substr(0, enum_t.find(";"));
    sv enum_v = raw.substr(raw.find(value_find) + value_find.size());
    if (enum_v.find("::")) {
        enum_v = enum_v.substr(enum_v.find_last_of("::") + 1);
    }
    enum_v = enum_v.substr(0, enum_v.find("]"));
    return {
        .type = enum_t,
        .name = enum_v,
        .raw = raw,
        .value = int(val),
    };
}
#else
static_assert(false, "platform not supported")
#endif
}
template <Enum Ty, int val>
consteval Enum_info enum_info() {
    return detail::enum_info<Ty, static_cast<Ty>(val)>();
}
template <Enum Ty, int val>
constexpr Enum_element<Ty> enum_element() {
    constexpr auto v = info<Ty, val>();
    return Enum_element<Ty>{
        .name = v.name,
        .value = val,
    };
}
template <Enum Ty, int size = count<Ty>()> 
consteval std::array<Enum_info, size> to_array() {
    std::array<Enum_info, size> arr{};
    unroll_for<size>([&arr](auto i) {
        arr[i] = enum_info<Ty, i>();
    });
    return arr;
}
template <Enum Ty, int size = count<Ty>()>
constexpr Enum_element<Ty> enum_element(const std::string_view name, const std::source_location& loc = std::source_location::current()) {
    namespace sr = std::ranges;
    constexpr std::array<Enum_info, size> array = to_array<Ty, size>();
    const auto found_it = sr::find_if(array, [&name](const Enum_info& e) {return e.name == name;});
    return {.name = found_it->name, .value = found_it->value};
}
}
