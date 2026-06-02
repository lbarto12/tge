#pragma once
// IWYU pragma: private, include <tge/input.h>

#include "../sync/Awaitable.h"
#include "Keyboard.h"
#include "Platform.h"
namespace tge {
class KeyBuffer : public async::Awaitable {
public:
    KeyBuffer(tge::Key k) : k(k) {}

    bool Ready() override { return state; }

    bool Await() override {
        bool d = tge::Keyboard::GetKeyDown(k);
        if (d && !kdown) {
            kdown = true;
            state = true;
            return true;
        }
        kdown = d;
        state = false;
        return false;
    }

private:
    Key k;
    bool kdown;
    bool state = false;
};
} // namespace tge
