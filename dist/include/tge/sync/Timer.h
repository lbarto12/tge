#pragma once
// IWYU pragma: private, include <tge/game.h>

#include <chrono>
#include <ratio>

#include "Awaitable.h"

namespace tge {
template <typename TimeUnit> struct Timer : public async::Awaitable {
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;

    Timer() : interval(TimeUnit{0}), last(Clock::now()) {}
    Timer(long long interval) : interval(TimeUnit{interval}), last(Clock::now()) {}

    void Start() { this->last = Clock::now(); }

    void Reset() { this->last = Clock::now(); }

    bool Ready() { return std::chrono::duration_cast<TimeUnit>(Clock::now() - last) >= interval; }

    bool Await() override {
        if (Ready()) {
            Reset();
            return true;
        }
        return false;
    }

private:
    TimeUnit interval;
    TimePoint last;
};
} // namespace tge
