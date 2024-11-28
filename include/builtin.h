#pragma once
#include <filesystem>
struct lua_State;
namespace builtin {
void register_file_path_type(lua_State* L);
void register_event_type(lua_State* L);
namespace files {
using Path = std::filesystem::path;
using Directory_entry = std::filesystem::directory_entry;
}
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
int fn_read_file(lua_State* L);
int fn_get_mouse_position(lua_State* L);
int fn_is_key_down(lua_State* L);
int fn_is_point_in_rect(lua_State* L);
int files_module(lua_State* L);
}
