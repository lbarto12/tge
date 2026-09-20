#pragma once
// IWYU pragma: private, include <tge/game.h>

namespace tge::async {
struct Awaitable {
    virtual bool Ready() = 0;
    virtual bool Await() = 0;
    virtual ~Awaitable() = default;
};
inline bool Await(Awaitable* a) {
    if (a->Ready()) {
        return a->Await();
    }
    return false;
}
} // namespace tge::async
