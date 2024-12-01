#pragma once
#include "halia_api.hpp"
#include <filesystem>
#include <lualib.h>
#include <luaconf.h>
namespace halia {
namespace core {
    struct HALIA_API Library_entry {
        const char* name;
        lua_CFunction loader;
    };
    struct HALIA_API Launch_options {
        std::filesystem::path main_entry_point{"main.luau"};
        std::filesystem::path bin_path;
    };
    HALIA_API int bootstrap(Launch_options opts = {});
    HALIA_API lua_State* lua_state();
    HALIA_API void add_library(const Library_entry& entry);
}
}
