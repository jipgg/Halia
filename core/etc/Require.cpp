// This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
#include "Require.hpp"


#include "FileUtils.hpp"
#include "Luau/Common.h"

#include <algorithm>
#include <array>
#include <utility>
using namespace file_utils;

RequireResolver::RequireResolver(lua_State* L, std::string path)
    : path_to_resolve(std::move(path))
    , L(L) {
    lua_Debug ar;
    lua_getinfo(L, 1, "s", &ar);
    source_chunkname = ar.source;
    if (!is_require_allowed(source_chunkname))
        luaL_errorL(L, "require is not supported in this context");
    if (is_absolute_path(path_to_resolve))
        luaL_argerrorL(L, 1, "cannot require an absolute path");
    std::replace(path_to_resolve.begin(), path_to_resolve.end(), '\\', '/');
    substitute_alias_if_present(path_to_resolve);
}
[[nodiscard]] RequireResolver::ResolvedRequire RequireResolver::resolve_require(lua_State* L, std::string path) {
    RequireResolver resolver(L, std::move(path));
    module_status status = resolver.find_module();
    if (status != module_status::file_read)
        return ResolvedRequire{status};
    else
        return ResolvedRequire{status, std::move(resolver.chunkname), std::move(resolver.absolute_path), std::move(resolver.source_code)};
}
RequireResolver::module_status RequireResolver::find_module() {
    resolve_and_store_default_paths();
    // Put _MODULES table on stack for checking and saving to the cache
    luaL_findtable(L, LUA_REGISTRYINDEX, "_MODULES", 1);
    RequireResolver::module_status moduleStatus = find_module_impl();
    if (moduleStatus != RequireResolver::module_status::not_found)
        return moduleStatus;
    if (!should_search_paths_array())
        return moduleStatus;
    if (!is_config_fully_resolved)
        parse_next_config();
    // Index-based iteration because std::iterator may be invalidated if config.paths is reallocated
    for (size_t i = 0; i < config.paths.size(); ++i) {
        // "placeholder" acts as a requiring file in the relevant directory
        std::optional<std::string> absolutePathOpt = resolve_path(path_to_resolve, join_paths(config.paths[i], "placeholder"));
        if (!absolutePathOpt)
            luaL_errorL(L, "error requiring module");
        chunkname = *absolutePathOpt;
        absolute_path = *absolutePathOpt;
        moduleStatus = find_module_impl();
        if (moduleStatus != RequireResolver::module_status::not_found)
            return moduleStatus;
        // Before finishing the loop, parse more config files if there are any
        if (i == config.paths.size() - 1 && !is_config_fully_resolved)
            parse_next_config(); // could reallocate config.paths when paths are parsed and added
    }
    return RequireResolver::module_status::not_found;
}
RequireResolver::module_status RequireResolver::find_module_impl() {
    static const std::array<const char*, 4> possibleSuffixes = {".luau", ".lua", "/init.luau", "/init.lua"};
    size_t unsuffixedAbsolutePathSize = absolute_path.size();
    for (const char* possibleSuffix : possibleSuffixes) {
        absolute_path += possibleSuffix;
        // Check cache for module
        lua_getfield(L, -1, absolute_path.c_str());
        if (!lua_isnil(L, -1)) {
            return module_status::cached;
        }
        lua_pop(L, 1);
        // Try to read the matching file
        std::optional<std::string> source = read_file(absolute_path);
        if (source) {
            chunkname = "=" + chunkname + possibleSuffix;
            source_code = *source;
            return module_status::file_read;
        }
        absolute_path.resize(unsuffixedAbsolutePathSize); // truncate to remove suffix
    }
    return module_status::not_found;
}
bool RequireResolver::is_require_allowed(std::string_view sourceChunkname) {
    LUAU_ASSERT(!sourceChunkname.empty());
    return (sourceChunkname[0] == '=' || sourceChunkname[0] == '@');
}
bool RequireResolver::should_search_paths_array() {
    return !is_absolute_path(path_to_resolve) && !is_explicitly_relative(path_to_resolve);
}
void RequireResolver::resolve_and_store_default_paths() {
    if (!is_absolute_path(path_to_resolve)) {
        std::string chunknameContext = get_requiring_context_relative();
        std::optional<std::string> absolutePathContext = get_requiring_context_absolute();
        if (!absolutePathContext)
            luaL_errorL(L, "error requiring module");
        // resolvePath automatically sanitizes/normalizes the paths
        std::optional<std::string> chunknameOpt = resolve_path(path_to_resolve, chunknameContext);
        std::optional<std::string> absolutePathOpt = resolve_path(path_to_resolve, *absolutePathContext);
        if (!chunknameOpt || !absolutePathOpt)
            luaL_errorL(L, "error requiring module");
        chunkname = std::move(*chunknameOpt);
        absolute_path = std::move(*absolutePathOpt);
    } else {
        // Here we must explicitly sanitize, as the path is taken as is
        std::optional<std::string> sanitizedPath = normalize_path(path_to_resolve);
        if (!sanitizedPath)
            luaL_errorL(L, "error requiring module");
        chunkname = *sanitizedPath;
        absolute_path = std::move(*sanitizedPath);
    }
}
std::optional<std::string> RequireResolver::get_requiring_context_absolute() {
    std::string requiringFile;
    if (is_absolute_path(source_chunkname.substr(1))) {
        // We already have an absolute path for the requiring file
        requiringFile = source_chunkname.substr(1);
    } else {
        // Requiring file's stored path is relative to the CWD, must make absolute
        std::optional<std::string> cwd = getCurrentWorkingDirectory();
        if (!cwd)
            return std::nullopt;
        if (source_chunkname.substr(1) == "stdin") {
            // Require statement is being executed from REPL input prompt
            // The requiring context is the pseudo-file "stdin" in the CWD
            requiringFile = join_paths(*cwd, "stdin");
        } else {
            // Require statement is being executed in a file, must resolve relative to CWD
            std::optional<std::string> requiringFileOpt = resolve_path(source_chunkname.substr(1), join_paths(*cwd, "stdin"));
            if (!requiringFileOpt)
                return std::nullopt;
            requiringFile = *requiringFileOpt;
        }
    }
    std::replace(requiringFile.begin(), requiringFile.end(), '\\', '/');
    return requiringFile;
}
std::string RequireResolver::get_requiring_context_relative() {
    std::string baseFilePath;
    if (source_chunkname.substr(1) != "stdin")
        baseFilePath = source_chunkname.substr(1);
    return baseFilePath;
}

void RequireResolver::substitute_alias_if_present(std::string& path) {
    if (path.size() < 1 || path[0] != '@')
        return;
    // To ignore the '@' alias prefix when processing the alias
    const size_t aliasStartPos = 1;
    // If a directory separator was found, the length of the alias is the
    // distance between the start of the alias and the separator. Otherwise,
    // the whole string after the alias symbol is the alias.
    size_t aliasLen = path.find_first_of("\\/");
    if (aliasLen != std::string::npos)
        aliasLen -= aliasStartPos;
    const std::string potentialAlias = path.substr(aliasStartPos, aliasLen);
    // Not worth searching when potentialAlias cannot be an alias
    if (!Luau::isValidAlias(potentialAlias))
        luaL_errorL(L, "@%s is not a valid alias", potentialAlias.c_str());
    std::optional<std::string> alias = get_alias(potentialAlias);
    if (alias) {
        path = *alias + path.substr(potentialAlias.size() + 1);
    } else {
        luaL_errorL(L, "@%s is not a valid alias", potentialAlias.c_str());
    }
}
std::optional<std::string> RequireResolver::get_alias(std::string alias) {
    std::transform(
        alias.begin(),
        alias.end(),
        alias.begin(),
        [](unsigned char c) {
            return ('A' <= c && c <= 'Z') ? (c + ('a' - 'A')) : c;
        }
    );
    while (!config.aliases.count(alias) && !is_config_fully_resolved) {
        parse_next_config();
    }
    if (!config.aliases.count(alias) && is_config_fully_resolved)
        return std::nullopt; // could not find alias
    return resolve_path(config.aliases[alias], join_paths(last_searched_dir, Luau::kConfigName));
}
void RequireResolver::parse_next_config() {
    if (is_config_fully_resolved)
        return; // no config files left to parse

    std::optional<std::string> directory;
    if (last_searched_dir.empty())
    {
        std::optional<std::string> requiringFile = get_requiring_context_absolute();
        if (!requiringFile)
            luaL_errorL(L, "error requiring module");

        directory = get_parent_path(*requiringFile);
    }
    else
        directory = get_parent_path(last_searched_dir);

    if (directory)
    {
        last_searched_dir = *directory;
        parse_config_in_directory(*directory);
    }
    else
        is_config_fully_resolved = true;
}

void RequireResolver::parse_config_in_directory(const std::string& directory)
{
    std::string configPath = join_paths(directory, Luau::kConfigName);

    size_t numPaths = config.paths.size();

    if (std::optional<std::string> contents = read_file(configPath))
    {
        std::optional<std::string> error = Luau::parseConfig(*contents, config);
        if (error)
            luaL_errorL(L, "error parsing %s (%s)", configPath.c_str(), (*error).c_str());
    }

    // Resolve any newly obtained relative paths in "paths" in relation to configPath
    for (auto it = config.paths.begin() + numPaths; it != config.paths.end(); ++it)
    {
        if (!is_absolute_path(*it))
        {
            if (std::optional<std::string> resolvedPath = resolve_path(*it, configPath))
                *it = std::move(*resolvedPath);
            else
                luaL_errorL(L, "error requiring module");
        }
    }
}
