#pragma once
#include <string_view>
struct lua_State;

struct Reader {
    virtual std::string_view read_word() = 0;
    virtual std::string_view read_line() = 0;
};

struct Writer {
    virtual void write(std::string_view data) = 0;
};
