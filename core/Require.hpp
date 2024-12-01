// This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
#pragma once

#include <lua.h>
#include <lualib.h>

#include "Luau/Config.h"

#include <string>
#include <string_view>

class RequireResolver {
public:
    std::string chunkname;
    std::string absolute_path;
    std::string source_code;
    enum class module_status {
        cached,
        file_read,
        not_found
    };
    struct ResolvedRequire {
        module_status status;
        std::string chunkname;
        std::string absolute_path;
        std::string source_code;
    };
    [[nodiscard]] ResolvedRequire static resolve_require(lua_State* L, std::string path);
private:
    std::string path_to_resolve;
    std::string_view source_chunkname;
    RequireResolver(lua_State* L, std::string path);
    module_status find_module();
    lua_State* L;
    Luau::Config config;
    std::string last_searched_dir;
    bool is_config_fully_resolved = false;

    bool is_require_allowed(std::string_view sourceChunkname);
    bool should_search_paths_array();

    void resolve_and_store_default_paths();
    module_status find_module_impl();

    std::optional<std::string> get_requiring_context_absolute();
    std::string get_requiring_context_relative();

    void substitute_alias_if_present(std::string& path);
    std::optional<std::string> get_alias(std::string alias);

    void parse_next_config();
    void parse_config_in_directory(const std::string& path);
};
