//common
#pragma once
#include <cassert>
#include <optional>
#include <utility>
#include <iostream>
#include <functional>
#include <filesystem>
namespace common {
struct Scope_guard {
    using Defer = std::function<void()>;
    std::unique_ptr<Defer> defer;
    Scope_guard(std::function<void()>&& defer): defer(std::make_unique<Defer>(std::forward<Defer>(defer))) {} 
    Scope_guard(): defer(nullptr) {}
    ~Scope_guard() {(*defer)();};
};
}
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
