#pragma once
#if defined(_WIN32) || defined(_WIN64)
    #ifdef BUILD_HALIA_CORE_DLL
        #define HALIA_CORE_API __declspec(dllexport)
    #else
        #define HALIA_CORE_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #ifdef BUILD_HALIA_CORE_DLL
        #define HALIA_CORE_API __attribute__((visibility("default")))
    #else
        #define HALIA_CORE_API
    #endif
#else
    #define HALIA_CORE_API
    #pragma warning Unknown dynamic link import/export semantics
#endif
