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
    struct HALIA_CORE_API Library_entry {
        const char* name;
        lua_CFunction loader;
    };
    struct HALIA_CORE_API Launch_options {
        std::filesystem::path main_entry_point{"main.luau"};
        std::filesystem::path bin_path;
        std::span<std::string_view> args;
    };
    using State = lua_State*;
    using Co_thread = lua_State*;
    [[nodiscard]] HALIA_CORE_API std::span<std::string_view> args_span() noexcept;
    [[nodiscard("error message on failure")]] HALIA_CORE_API std::optional<Error_info> init(const Launch_options& opts = {}) noexcept;
    [[nodiscard("exit code")]] HALIA_CORE_API int bootstrap(const Launch_options& opts = {});
    [[nodiscard]] HALIA_CORE_API State state() noexcept;
    HALIA_CORE_API void add_library(const Library_entry& entry);
    [[nodiscard]] HALIA_CORE_API Co_thread main_thread() noexcept;
}
}
