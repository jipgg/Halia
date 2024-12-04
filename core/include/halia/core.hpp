#pragma once
#include "halia_core_api.hpp"
#include <filesystem>
#include <lualib.h>
#include <luaconf.h>
#include <span>
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
    HALIA_CORE_API std::span<std::string_view> args_span() noexcept;
    HALIA_CORE_API int bootstrap(Launch_options opts = {});
    HALIA_CORE_API lua_State* lua_state() noexcept;
    HALIA_CORE_API void add_library(const Library_entry& entry);
    HALIA_CORE_API lua_State* main_thread() noexcept;
}
}
