#pragma once
#include "halia_core_api.hpp"
#include <filesystem>
#include <lualib.h>
#include <luaconf.h>
#include <span>
#include <optional>
#include "Error_info.hpp"
namespace halia {
namespace core {
    struct HALIA_CORE_API LibraryEntry {
        const char* name;
        lua_CFunction loader;
    };
    struct HALIA_CORE_API LaunchOptions {
        std::filesystem::path main_entry_point{"main.luau"};
        std::filesystem::path bin_path;
        std::span<std::string_view> args;
    };
    using State = lua_State*;
    using CoThread = lua_State*;
    [[nodiscard]] HALIA_CORE_API std::span<std::string_view> args_span() noexcept;
    [[nodiscard("error message on failure")]] HALIA_CORE_API std::optional<ErrorInfo> init(const LaunchOptions& opts = {}) noexcept;
    [[nodiscard("exit code")]] HALIA_CORE_API int bootstrap(const LaunchOptions& opts = {});
    [[nodiscard]] HALIA_CORE_API State state() noexcept;
    HALIA_CORE_API void add_library(const LibraryEntry& entry);
    [[nodiscard]] HALIA_CORE_API CoThread main_thread() noexcept;
}
}
