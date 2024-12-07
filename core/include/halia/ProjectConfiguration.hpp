#pragma once
#include "halia_core_api.hpp"
#include "ErrorInfo.hpp"
#include <optional>
#include <vector>
#include <filesystem>
#include <lua.h>
#include <variant>
#include <bitset>

namespace halia {
struct HALIA_CORE_API ProjectConfiguration {
    enum class Runtime {console, desktop};
    enum class Flags {testing, debug, _last};
    Runtime runtime;
    std::bitset<static_cast<size_t>(Flags::_last)> flags;
    std::vector<std::filesystem::path> script_paths;
};
HALIA_CORE_API std::variant<ProjectConfiguration, ErrorInfo> parse_project_configuration_file(const std::filesystem::path& config_file);
}
