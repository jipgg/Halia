//common
#pragma once
#include <cassert>
#include <optional>
#include <utility>
#include <iostream>
#include <functional>
#include <filesystem>
struct Scope_guard {
    using Defer_function = std::function<void()>;
    Defer_function block;
    ~Scope_guard() {block();};
};
//concepts
template <class Ty>
concept Comparable_to_nullptr = std::is_convertible_v<decltype(std::declval<Ty>() != nullptr), bool>;
//free functions
#ifdef _WIN32
void attach_console();
void enable_ansi_escape_sequences();
#endif
[[nodiscard]] std::optional<std::string> read_file(const std::filesystem::path& path);
template <class ...Ts>
void print(Ts&&...args) {
    ((std::cout << args << ' '), ...) << '\n';
}
template <class ...Ts>
void printerr(Ts&&...args) {
    std::cerr << "\033[31m";//red
    ((std::cerr << args << ' '), ...) << "\033[0m\n";
}
