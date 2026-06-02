#pragma once
// IWYU pragma: private, include <tge/game.h>

#include "Awaitable.h"
#include <initializer_list>
#include <vector>

namespace tge::async {
struct AwaitGroup : public Awaitable {
    AwaitGroup(std::initializer_list<Awaitable*> awaits) : awaits(awaits) {}

    bool Ready() override {
        for (Awaitable* await : awaits)
            if (!await->Ready()) return false;
        return true;
    }

    bool Await() override {
        // Check first
        if (!this->Ready()) return false;

        // Then consume all
        for (Awaitable* await : awaits) await->Await();

        return true;
    }

private:
    std::vector<Awaitable*> awaits;
};
} // namespace tge::async
