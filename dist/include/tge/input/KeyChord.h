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

    bool Ready() override { return g.Ready(); }

    bool Await() override { return g.Await(); }

private:
    // Need to store to pass stack pointers to await group. mem managed by design.
    std::vector<tge::KeyHold> buffs;
    async::AwaitGroup g = {};
};
} // namespace tge
