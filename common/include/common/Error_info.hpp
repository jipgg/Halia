#pragma once
#include <source_location>
#include <vector>
#include <span>
#include <string>
#include <string_view>
#include <format>
#include <iostream>
#include <ranges>

class Error_info {
    std::vector<std::source_location> traceback_;
    std::string message_;
public:
    Error_info(const Error_info& a) = delete; 
    Error_info& operator=(const Error_info& a) = delete;
    Error_info(Error_info&& a) noexcept = default;
    Error_info& operator=(Error_info& a) noexcept = default;
    ~Error_info() = default;
    explicit Error_info(std::string message, std::source_location sl = std::source_location::current()) noexcept:
        message_(std::move(message)) {
        traceback_.emplace_back(std::move(sl));
    }
    Error_info propagate(std::source_location sl = std::source_location::current()) {
        traceback_.emplace_back(sl);
        return std::move(*this);
    }
    const std::vector<std::source_location>& traceback() const {
        return traceback_;
    }
    const std::string& message() const {
        return message_;
    }
    std::string formatted() const {
        std::string ret =  std::format("Error \"{}\":\n", message_);
        for (const auto& sl : traceback_ | std::views::reverse) {
            ret += std::format(
                " -> {}\n"
                "        in function: {}\n",
                sl.line(), sl.column(), sl.file_name(), sl.function_name());
        }
        return ret;
    }
    friend std::ostream& operator<<(std::ostream& os, const Error_info& error) {
        return os << error.formatted();
    }
};
