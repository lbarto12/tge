#pragma once
// IWYU pragma: private, include <tge/game.h>

#include "Awaitable.h"
#include <initializer_list>
#include <vector>

namespace tge::sync {
struct AwaitGroup : public awaits::Awaitable {
    AwaitGroup(std::initializer_list<Awaitable*> awaits) : awaits(awaits) {}

    bool Await() override {
        for (Awaitable* await : awaits)
            if (!await->Await()) return false;

        return true;
    }

private:
    std::vector<Awaitable*> awaits;
};
} // namespace tge::sync
