#include "Error_info.hpp"
#include <format>
#include <ranges>
#include <filesystem>

namespace halia {
ErrorInfo::ErrorInfo(std::string message, std::source_location sl):
        message_(std::move(message)) {
        traceback_.emplace_back(std::move(sl));
    }
ErrorInfo& ErrorInfo::propagate(std::source_location sl) {
        traceback_.emplace_back(sl);
        return *this;
    }
std::string ErrorInfo::formatted() const {
        std::string ret =  std::format("Error \"{}\"\n", message_);
        for (const auto& sl : traceback_ | std::views::reverse) {
            const std::string filename = std::filesystem::path(sl.file_name()).filename().string();
            std::string_view fname = sl.function_name();
            const bool caused_here = fname == traceback_.front().function_name();
            const std::string_view symbol = caused_here ? "*" : "^";
#if defined (_MSC_VER)
            constexpr std::string_view cdecl_keyw = "__cdecl ";
            fname = fname.substr(fname.find(cdecl_keyw) + cdecl_keyw.size());
            fname = fname.substr(0, fname.find("("));
#endif
            ret += std::format(
                "  {} {}:{}: in function '{}'\n",
                symbol, filename, sl.line(), fname);
        }
        ret.pop_back();
        return ret;
    }
std::ostream& operator<<(std::ostream& os, const ErrorInfo& error) {
    return os << error.formatted();
}
const std::vector<std::source_location>& ErrorInfo::traceback() const {return traceback_;}
const std::string& ErrorInfo::message() const {return message_;}
}
