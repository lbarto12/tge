#pragma once
// IWYU pragma: private, include <tge/game.h>

#include <chrono>
#include <functional>
#include <future>

#include "./Awaitable.h"

namespace tge::async {

template <typename Args, typename Return> struct Thread : public Awaitable {
    Thread(std::function<Return(Args)> f) : Awaitable(), f(f) {}

    void Start(Args args) {
        this->consumed = false;
        this->future = std::async(std::launch::async, f, args);
    }

    bool Ready() override {
        return !this->consumed && this->future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    }

    bool Await() override {
        if (!consumed && this->Ready()) {
            this->consumed = true;
            return true;
        }
        return false;
    }

    Return Result() { return this->future.get(); }

private:
    bool consumed = false;
    std::function<Return(Args)> f;
    std::future<Return> future;
};
} // namespace tge::async
