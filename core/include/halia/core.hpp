#pragma once
#include "halia_api.hpp"
#include <filesystem>
#include <lualib.h>
#include <luaconf.h>
#include <span>
#include "Cothread.hpp"
namespace halia {
namespace core {
    struct HALIA_API Library_entry {
        const char* name;
        lua_CFunction loader;
    };
    struct HALIA_API Launch_options {
        std::filesystem::path main_entry_point{"main.luau"};
        std::filesystem::path bin_path;
        std::span<std::string_view> args;
    };
    HALIA_API std::span<std::string_view> args_span() noexcept;
    HALIA_API int bootstrap(Launch_options opts = {});
    HALIA_API lua_State* lua_state() noexcept;
    HALIA_API void add_library(const Library_entry& entry);
    HALIA_API void emplace_cothread(Cothread&& co);
    HALIA_API Cothread* find_cothread(lua_State* thread_state) noexcept; 
}
}
