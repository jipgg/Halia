#pragma once
#include <string>
#include <filesystem>
#include <source_location>
struct lua_State;
namespace runtime {
struct Launch_options {
    std::filesystem::path main_entry_point{"main.luau"};
    std::filesystem::path bin_path;
};
int bootstrap(Launch_options opts = {});
void quit();
lua_State* lua_state();
void expect(bool expression, std::string_view reason = "not specified", const std::source_location& location = std::source_location::current());
}
