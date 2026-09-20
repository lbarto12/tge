#pragma once
// IWYU pragma: private, include <tge/input.h>

#include "../sync/Awaitable.h"
#include "Keyboard.h"
#include "Platform.h"

namespace tge {
class KeyBuffer : public async::Awaitable {
public:
    KeyBuffer(tge::Key k) : k(k) {}

    bool Ready() override {
        bool d = tge::Keyboard::GetKeyDown(k);
        if (!d) consumed = false;
        return d && !consumed;
    }

    bool Await() override {
        if (Ready()) {
            consumed = true;
            return true;
        }
        return false;
    }

private:
    Key k;
    bool consumed = false;
};
} // namespace tge
