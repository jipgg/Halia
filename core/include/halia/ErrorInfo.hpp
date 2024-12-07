#pragma once
#include "halia_core_api.hpp"
#include <source_location>
#include <vector>
#include <string>
#include <iostream>

namespace halia {
class HALIA_CORE_API ErrorInfo {
    std::vector<std::source_location> traceback_;
    std::string message_;
public:
    ErrorInfo(const ErrorInfo& a) = default; 
    ErrorInfo& operator=(const ErrorInfo& a) = default;
    ErrorInfo(ErrorInfo&& a) noexcept = default;
    ErrorInfo& operator=(ErrorInfo& a) noexcept = default;
    ~ErrorInfo() = default;
    explicit ErrorInfo(std::string message, std::source_location sl = std::source_location::current());
    ErrorInfo& propagate(std::source_location sl = std::source_location::current());
    const std::vector<std::source_location>& traceback() const;
    const std::string& message() const;
    std::string formatted() const;
    HALIA_CORE_API friend std::ostream& operator<<(std::ostream& os, const ErrorInfo& error);
};
}
