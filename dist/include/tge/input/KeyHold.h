#pragma once
// IWYU pragma: private, include <tge/input.h>

#include "../input/Keyboard.h"
#include "../input/Platform.h"
#include "../sync/Awaitable.h"
namespace tge {
class KeyHold : public tge::async::Awaitable {
public:
    KeyHold(tge::Key k) : k(k) {}

    bool Ready() override { return tge::Keyboard::GetKeyDown(k); }

    bool Await() override { return tge::Keyboard::GetKeyDown(k); }

private:
    tge::Key k;
};
} // namespace tge
