// This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <functional>
#include <vector>

namespace file_utils {
std::optional<std::string> getCurrentWorkingDirectory();

std::string normalize_path(std::string_view path);
std::string resolve_path(std::string_view relativePath, std::string_view baseFilePath);

std::optional<std::string> read_file(const std::string& name);
std::optional<std::string> read_stdin();

bool is_absolute_path(std::string_view path);
bool is_explicitly_relative(std::string_view path);
bool is_directory(const std::string& path);
bool traverse_directory(const std::string& path, const std::function<void(const std::string& name)>& callback);

std::vector<std::string_view> split_path(std::string_view path);
std::string join_paths(const std::string& lhs, const std::string& rhs);
std::optional<std::string> get_parent_path(const std::string& path);

std::vector<std::string> get_source_files(int argc, char** argv);
}
