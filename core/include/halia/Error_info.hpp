#pragma once
#include "halia_core_api.hpp"
#include <source_location>
#include <vector>
#include <string>
#include <iostream>

namespace halia {
class HALIA_CORE_API Error_info {
    std::vector<std::source_location> traceback_;
    std::string message_;
public:
    Error_info(const Error_info& a) = default; 
    Error_info& operator=(const Error_info& a) = default;
    Error_info(Error_info&& a) noexcept = default;
    Error_info& operator=(Error_info& a) noexcept = default;
    ~Error_info() = default;
    explicit Error_info(std::string message, std::source_location sl = std::source_location::current());
    Error_info& propagate(std::source_location sl = std::source_location::current());
    const std::vector<std::source_location>& traceback() const;
    const std::string& message() const;
    std::string formatted() const;
    HALIA_CORE_API friend std::ostream& operator<<(std::ostream& os, const Error_info& error);
};
}
