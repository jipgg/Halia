#pragma once
#include <lualib.h>
#include "CoreLibrary.hpp"
#include <vector>
#include <utility>

namespace library {
extern CoreLibrary filesystem;
extern CoreLibrary math;
extern CoreLibrary process;
extern CoreLibrary io;
extern CoreLibrary datetime;
int co_spawn(lua_State* L);
int co_wait(lua_State* L);
struct Event {
    struct Connection {
        size_t id;
    };
    std::vector<std::pair<size_t, int>> refs;
    lua_State* L;
    static constexpr size_t nullid = 0;
    size_t curr_id{nullid};
    Event(lua_State* L);
    ~Event();
    Connection connect(int idx);
    void disconnect(Connection id);
    void fire(int arg_count);
};
int event_ctor(lua_State* L);
void register_event_type(lua_State* L);
}

