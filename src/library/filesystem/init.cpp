#include "init.h"
#include "util.h"
#include "common.h"
#include "library.h"
using util::resolve_path_type;

static fs::copy_options to_copy_options(std::string_view str) {
    using copts = fs::copy_options;
    copts opt;
    if (str == "Recursive") opt = copts::recursive;
    else if (str == "Copy Symlinks") opt = copts::copy_symlinks;
    else if (str == "Skip Symlinks") opt = copts::skip_symlinks;
    else if (str == "Skip Existing") opt = copts::skip_existing;
    else if (str == "Update Existing") opt = copts::update_existing;
    else if (str == "Create Symlinks") opt = copts::create_symlinks;
    else if (str == "Directories Only") opt = copts::directories_only;
    else if (str == "Create Hard Links") opt = copts::create_hard_links;
    else if (str == "Overwrite Existing") opt = copts::overwrite_existing;
    else opt = copts::none;
    return opt;
}
static fs::file_type to_file_type(std::string_view str) {
    using ft = fs::file_type;
    ft t;
    if (str == "None") t = ft::none;
    else if (str == "Junction") t = ft::junction;
    else if (str == "Fifo") t = ft::fifo;
    else if (str == "Block") t = ft::block;
    else if (str == "Socket") t = ft::socket;
    return t;
}
static int create_directory(lua_State* L) {
    if (auto path = resolve_path_type(L, 1)) {
        if (fs::create_directory(*path)) lua_pushboolean(L, true);
        else lua_pushboolean(L, false);
        return 1;
    } else {
        luaL_error(L, "unsupported type");
        return 0;
    }
}
static int exists(lua_State* L) {
    if (auto path = resolve_path_type(L, 1)) {
        lua_pushboolean(L, fs::exists(*path));
        return 1;
    }
    luaL_error(L, "unsupported type");
    return 0;
}
static int is_character_file(lua_State* L) {
    if (auto path = resolve_path_type(L, 1)) {
        lua_pushboolean(L, fs::is_character_file(*path));
        return 1;
    }
    luaL_error(L, "unsupported type");
    return 0;
}
static int read_text_file(lua_State* L) {
    auto path = resolve_path_type(L, 1);
    if (not path) return 0;
    auto contents = read_file(*path);
    if (not contents) return 0;
    lua_pushlstring(L, contents->data(), contents->size());
    return 1;
}
static int write_text_file(lua_State* L) {
    lua_pushboolean(L, true);
    return 1;
}
static int copy_file(lua_State* L) {
    auto from = resolve_path_type(L, 1);
    auto to = resolve_path_type(L, 2);
    if (from and to) {
        lua_pushboolean(L, fs::copy_file(*from, *to, to_copy_options(luaL_optstring(L, 3, "none"))));
        return 1;
    }
    return 0;
}
static int rename(lua_State* L) {
    auto from = resolve_path_type(L, 1);
    auto to = resolve_path_type(L, 2);
    if (from and to) {
        fs::rename(*from, *to);
    } else {
        printerr("not renamed");
    }
    return 0;
}
static int remove(lua_State* L) {
    auto path = resolve_path_type(L, 1);
    assert(path.has_value());
    lua_pushboolean(L, fs::remove(*path));
    return 1;
}
static int remove_all(lua_State* L) {
    auto path = resolve_path_type(L, 1);
    assert(path.has_value());
    lua_pushnumber(L, fs::remove_all(*path));
    return 1;
}
static int copy(lua_State* L) {
    auto src = resolve_path_type(L, 1);
    auto dest = resolve_path_type(L, 2);
    assert(src.has_value() and dest.has_value());
    std::string_view str = luaL_optstring(L, 3, "none");
    fs::copy(*src, *dest, to_copy_options(str));
    return 0;
}
static int is_directory(lua_State* L) {
    auto path = resolve_path_type(L, 1);
    assert(path.has_value());
    lua_pushboolean(L, fs::is_directory(luaL_checkstring(L, 1)));
    return 1;
}
static int absolute(lua_State* L) {
    auto path = resolve_path_type(L, 1);
    assert(path.has_value());
    create<fs::path>(L, fs::absolute(*path));
    return 1;
}
static int is_empty(lua_State* L) {
    auto path = resolve_path_type(L, 1);
    assert(path.has_value());
    lua_pushboolean(L, fs::is_empty(*path));
    return 1;
}
static int children_of(lua_State* L) {
    auto path = resolve_path_type(L, 1);
    assert(path.has_value());
    lua_newtable(L);
    if (path->empty()) return 1;
    int i{};
    for (auto& entry : fs::directory_iterator(*path)) {
        ++i;
        lua_pushinteger(L, i);
        create<fs::directory_entry>(L, entry);
        lua_settable(L, -3);
    }
    return 1;
}
static int descendants_of(lua_State* L) {
    auto path = resolve_path_type(L, 1);
    assert(path.has_value());
    lua_newtable(L);
    if (path->empty()) return 1;
    int i{};
    for (auto& entry : fs::recursive_directory_iterator(*path)) {
        ++i;
        lua_pushinteger(L, i);
        create<fs::directory_entry>(L, entry);
        lua_settable(L, -3);
    }
    return 1;
}
static int executable_directory(lua_State* L) {
    create<Path>(L, util::get_executable_path());
    return 1;
}
static int current_working_directory(lua_State* L) {
    create<Path>(L, fs::current_path());
    return 1;
}
static int canonical(lua_State* L) {
    create<Path>(L, fs::canonical(*resolve_path_type(L, 1)));
    return 1;
}
static int proximate(lua_State* L) {
    auto base_opt = resolve_path_type(L, 2);
    create<Path>(L, fs::proximate(*resolve_path_type(L, 1),
        base_opt ? *base_opt : fs::current_path()));

    return 1;
}
static int create_symlink(lua_State* L) {
    fs::create_symlink(*resolve_path_type(L, 1), *resolve_path_type(L, 2));
    return 0;
}
static int relative(lua_State* L) {
    auto base_opt = resolve_path_type(L, 2);
    create<Path>(L, fs::relative(
                 *resolve_path_type(L, 1),
                 base_opt? *base_opt : fs::current_path()));
    return 1;
}

static const luaL_Reg filesystem_table[] = {
    {"create_directory", create_directory},
    {"exists", exists},
    {"is_character_file", is_character_file},
    {"copy_file", copy_file},
    {"rename", rename},
    {"remove", remove},
    {"remove_all", remove_all},
    {"copy", copy},
    {"is_directory", is_directory},
    {"absolute", absolute},
    {"is_empty", is_empty},
    {"get_children_of", children_of},
    {"get_descendants_of", descendants_of},
    {"exe_path", executable_directory},
    {"current_path", current_working_directory},
    {"canonical", canonical},
    {"proximate", proximate},
    {"create_symlink", create_symlink},
    {"Path", exported::path_ctor},
    {"relative", relative},
    {"read_file", read_text_file},
    {"write_file", write_text_file},
    {nullptr, nullptr}
};
namespace library {
Builtin_library filesystem{"filesystem", [](lua_State* L) {
    exported::init_directory_entry_meta(L);
    exported::init_path_meta(L);
    lua_newtable(L);
    luaL_register(L, nullptr, filesystem_table);
    return 1;
}};
}

