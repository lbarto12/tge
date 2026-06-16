#pragma once
// IWYU pragma: private, include <tge/input.h>

#include "../sync/AwaitGroup.h"
#include "../sync/Awaitable.h"
#include "KeyHold.h"
#include "Keyboard.h"
#include "Platform.h"
#include <initializer_list>
#include <vector>

namespace tge {
class KeyChord : public async::Awaitable {
public:
    KeyChord(std::initializer_list<tge::Key> init) {
        for (auto k : init) {
            buffs.push_back(k);
        }
        for (auto& kh : buffs) {
            g.Add(&kh);
        }
    }

    bool Ready() override {
        if (!g.Ready()) consumed = false;
        return g.Ready() && !consumed;
    }

    bool Await() override {
        if (g.Ready()) {
            consumed = true;
            return true;
        }
        return false;
    }

private:
    // Need to store to pass stack pointers to await group. mem managed by design.
    std::vector<tge::KeyHold> buffs;
    async::AwaitGroup g = {};
    bool consumed = false;
};
} // namespace tge
