#pragma once
// IWYU pragma: private, include <tge/input.h>

#include "../sync/Awaitable.h"
#include "Keyboard.h"
#include "Platform.h"
namespace tge {
class KeyBuffer : public async::Awaitable {
public:
    KeyBuffer(tge::Key k) : k(k) {}

    bool Await() override {
        bool d = tge::Keyboard::GetKeyDown(k);
        if (d && !kdown) {
            kdown = true;
            return true;
        }
        kdown = d;
        return false;
    }

private:
    Key k;
    bool kdown;
};
} // namespace tge
