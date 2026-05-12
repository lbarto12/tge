#include "Platform.h"

#if defined(TGE_PLATFORM_POSIX)
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <sys/select.h>
#include <unistd.h>

namespace tge::platform {

static std::atomic<bool> g_focused{true};
static bool g_enabled = false;

static void disableFocusReporting() {
    std::fputs("\x1b[?1004l", stdout);
    std::fflush(stdout);
}

static void enableFocusReporting() {
    if (g_enabled) return;
    std::fputs("\x1b[?1004h", stdout);
    std::fflush(stdout);
    std::atexit(disableFocusReporting);
    g_enabled = true;
}

static void drainFocusEvents() {
    unsigned char buf[64];
    for (;;) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        struct timeval tv{0, 0};
        if (select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) <= 0) return;
        ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
        if (n <= 0) return;
        for (ssize_t i = 0; i + 2 < n; ++i) {
            if (buf[i] == 0x1b && buf[i + 1] == '[' && (buf[i + 2] == 'I' || buf[i + 2] == 'O')) {
                g_focused.store(buf[i + 2] == 'I', std::memory_order_relaxed);
                i += 2;
            }
        }
    }
}

bool isTerminalFocused() {
    enableFocusReporting();
    drainFocusEvents();
    return g_focused.load(std::memory_order_relaxed);
}

} // namespace tge::platform
#endif
