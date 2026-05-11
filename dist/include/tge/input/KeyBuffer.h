#pragma once
// IWYU pragma: private, include <tge/input.h>

#include "Keyboard.h"
#include "Platform.h"
namespace tge {
class KeyBuffer {
public:
    KeyBuffer(tge::Key k) : k(k) {}

    bool SinglePress() {
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
