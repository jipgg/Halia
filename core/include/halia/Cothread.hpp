#pragma once
#include "halia_api.hpp"
#include <chrono>
#include <lualib.h>
#include <optional>
#include <string>
#include <functional>

class HALIA_API Cothread {
    int ref_;
public:
    using Steady_clock = std::chrono::steady_clock;
    using Time_point = Steady_clock::time_point;
    using Duration = Steady_clock::duration; 
    std::unique_ptr<lua_State, std::function<void(lua_State*)>> thread;
    std::optional<std::string> error_message;
    Time_point yield_start{};
    Duration yield_duration{};
    enum class Status {
        waiting, finished, had_error, yielding, idling
    };
    Status status;
    Cothread(lua_State* L, Status status = Status::idling):
        status(status),
        thread(lua_newthread(L), [this, L](lua_State*){lua_unref(L, ref_);}) {
        ref_ = lua_ref(L, -1);
        lua_pop(L, 1);
    }
    lua_State* state() const noexcept {
        return thread.get();
    }
};

