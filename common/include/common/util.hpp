#pragma once
#include <filesystem>
#include <lualib.h>
#include <ranges>
#include <optional>
struct lua_State;
namespace util {
std::filesystem::path get_executable_path();

using ParamData = std::ranges::split_view<std::string_view, std::ranges::single_view<char>>;
static std::optional<ParamData> parse_param_data(std::string_view str) {
    auto start = str.find_first_of('=');
    if (start == std::string_view::npos) return std::nullopt;
    return std::views::split(str.substr(start), ',');
}
}
