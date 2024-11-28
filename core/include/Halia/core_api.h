#pragma once
#include <filesystem>
#include <lua.h>
#if defined(_WIN32) || defined(_WIN64)
    #ifdef HALIA_CORE_API_EXPORTS
        #define HALIA_CORE_API __declspec(dllexport)
    #else
        #define HALIA_CORE_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #ifdef HALIA_CORE_API_EXPORTS
        #define HALIA_CORE_API __attribute__((visibility("default")))
    #else
        #define HALIA_CORE_API
    #endif
#else
    #define HALIA_CORE_API
    #pragma warning Unknown dynamic link import/export semantics
#endif
struct lua_State;
namespace halia {
namespace core {
    struct HALIA_CORE_API Library_entry {
        const char* name;
        lua_CFunction loader;
    };
    struct HALIA_CORE_API Launch_options {
        std::filesystem::path main_entry_point{"main.luau"};
        std::filesystem::path bin_path;
    };
    HALIA_CORE_API int bootstrap(Launch_options opts = {});
    HALIA_CORE_API lua_State* lua_state();
    HALIA_CORE_API void add_library(const Library_entry& entry);
}
}
