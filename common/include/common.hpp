//common
#pragma once
#include <cassert>
#include <optional>
#include <utility>
#include <iostream>
#include <functional>
#include <filesystem>
#include <variant>
struct ScopeGuard {
    using Defer = std::function<void()>;
    std::unique_ptr<Defer> defer;
    ScopeGuard(std::function<void()>&& defer): defer(std::make_unique<Defer>(std::forward<Defer>(defer))) {} 
    ScopeGuard(): defer(nullptr) {}
    ~ScopeGuard() {(*defer)();};
};
template <class T>
using ErrorMessageOr = std::variant<std::string, T>;
using ErrorMessageOnFailure = std::optional<std::string>;
//concepts
template <class Ty>
concept ComparableToNullptr = std::is_convertible_v<decltype(std::declval<Ty>() != nullptr), bool>;
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
