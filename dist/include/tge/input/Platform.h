#pragma once
#include <optional>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <conio.h>
#include <windows.h>
#elif defined(__APPLE__)
#include <ApplicationServices/ApplicationServices.h>
#include <sys/select.h>
#include <unistd.h>
#else
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <unistd.h>
#include <vector>
#endif

namespace tge {

enum class Key {
    None = 0,
    Character,

    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,

    Num0,
    Num1,
    Num2,
    Num3,
    Num4,
    Num5,
    Num6,
    Num7,
    Num8,
    Num9,

    Up,
    Down,
    Left,
    Right,

    Escape,
    Enter,
    Backspace,
    Delete,
    Home,
    End,
    Tab,
    Space,

    LeftShift,
    RightShift,
    LeftCtrl,
    RightCtrl,
    LeftAlt,
    RightAlt,

    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
};

namespace platform {

// ---- stdin char input (used by GetKeyPressed) ----

inline bool keyhit() {
#ifdef _WIN32
    return _kbhit() != 0;
#else
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    struct timeval tv = {0, 0};
    return select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) > 0;
#endif
}

inline std::optional<char> getKey() {
    if (!keyhit()) return std::nullopt;
#ifdef _WIN32
    return static_cast<char>(_getch());
#else
    char c;
    if (read(STDIN_FILENO, &c, 1) == 1) return c;
    return std::nullopt;
#endif
}

inline std::optional<char> readByteTimeout() {
#ifdef _WIN32
    if (_kbhit()) return static_cast<char>(_getch());
    return std::nullopt;
#else
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    struct timeval tv = {0, 50'000};
    if (select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) > 0) {
        char c;
        if (read(STDIN_FILENO, &c, 1) == 1) return c;
    }
    return std::nullopt;
#endif
}

// ---- key-down state (used by GetKeyDown) ----

#ifdef _WIN32

inline int keyToNative(Key k) {
    switch (k) {
    case Key::A:
        return 'A';
    case Key::B:
        return 'B';
    case Key::C:
        return 'C';
    case Key::D:
        return 'D';
    case Key::E:
        return 'E';
    case Key::F:
        return 'F';
    case Key::G:
        return 'G';
    case Key::H:
        return 'H';
    case Key::I:
        return 'I';
    case Key::J:
        return 'J';
    case Key::K:
        return 'K';
    case Key::L:
        return 'L';
    case Key::M:
        return 'M';
    case Key::N:
        return 'N';
    case Key::O:
        return 'O';
    case Key::P:
        return 'P';
    case Key::Q:
        return 'Q';
    case Key::R:
        return 'R';
    case Key::S:
        return 'S';
    case Key::T:
        return 'T';
    case Key::U:
        return 'U';
    case Key::V:
        return 'V';
    case Key::W:
        return 'W';
    case Key::X:
        return 'X';
    case Key::Y:
        return 'Y';
    case Key::Z:
        return 'Z';
    case Key::Num0:
        return '0';
    case Key::Num1:
        return '1';
    case Key::Num2:
        return '2';
    case Key::Num3:
        return '3';
    case Key::Num4:
        return '4';
    case Key::Num5:
        return '5';
    case Key::Num6:
        return '6';
    case Key::Num7:
        return '7';
    case Key::Num8:
        return '8';
    case Key::Num9:
        return '9';
    case Key::Up:
        return VK_UP;
    case Key::Down:
        return VK_DOWN;
    case Key::Left:
        return VK_LEFT;
    case Key::Right:
        return VK_RIGHT;
    case Key::Escape:
        return VK_ESCAPE;
    case Key::Enter:
        return VK_RETURN;
    case Key::Backspace:
        return VK_BACK;
    case Key::Delete:
        return VK_DELETE;
    case Key::Home:
        return VK_HOME;
    case Key::End:
        return VK_END;
    case Key::Tab:
        return VK_TAB;
    case Key::Space:
        return VK_SPACE;
    case Key::LeftShift:
        return VK_LSHIFT;
    case Key::RightShift:
        return VK_RSHIFT;
    case Key::LeftCtrl:
        return VK_LCONTROL;
    case Key::RightCtrl:
        return VK_RCONTROL;
    case Key::LeftAlt:
        return VK_LMENU;
    case Key::RightAlt:
        return VK_RMENU;
    case Key::F1:
        return VK_F1;
    case Key::F2:
        return VK_F2;
    case Key::F3:
        return VK_F3;
    case Key::F4:
        return VK_F4;
    case Key::F5:
        return VK_F5;
    case Key::F6:
        return VK_F6;
    case Key::F7:
        return VK_F7;
    case Key::F8:
        return VK_F8;
    case Key::F9:
        return VK_F9;
    case Key::F10:
        return VK_F10;
    case Key::F11:
        return VK_F11;
    case Key::F12:
        return VK_F12;
    default:
        return 0;
    }
}

inline bool initKeyState() { return true; }
inline void shutdownKeyState() {}

inline bool isKeyDown(Key k) {
    int vk = keyToNative(k);
    if (!vk) return false;
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

#elif defined(__APPLE__)

// Stable HID keycodes; mirrors <HIToolbox/Events.h> kVK_* without pulling in Carbon.
inline int keyToNative(Key k) {
    switch (k) {
    case Key::A:
        return 0x00;
    case Key::B:
        return 0x0B;
    case Key::C:
        return 0x08;
    case Key::D:
        return 0x02;
    case Key::E:
        return 0x0E;
    case Key::F:
        return 0x03;
    case Key::G:
        return 0x05;
    case Key::H:
        return 0x04;
    case Key::I:
        return 0x22;
    case Key::J:
        return 0x26;
    case Key::K:
        return 0x28;
    case Key::L:
        return 0x25;
    case Key::M:
        return 0x2E;
    case Key::N:
        return 0x2D;
    case Key::O:
        return 0x1F;
    case Key::P:
        return 0x23;
    case Key::Q:
        return 0x0C;
    case Key::R:
        return 0x0F;
    case Key::S:
        return 0x01;
    case Key::T:
        return 0x11;
    case Key::U:
        return 0x20;
    case Key::V:
        return 0x09;
    case Key::W:
        return 0x0D;
    case Key::X:
        return 0x07;
    case Key::Y:
        return 0x10;
    case Key::Z:
        return 0x06;
    case Key::Num0:
        return 0x1D;
    case Key::Num1:
        return 0x12;
    case Key::Num2:
        return 0x13;
    case Key::Num3:
        return 0x14;
    case Key::Num4:
        return 0x15;
    case Key::Num5:
        return 0x17;
    case Key::Num6:
        return 0x16;
    case Key::Num7:
        return 0x1A;
    case Key::Num8:
        return 0x1C;
    case Key::Num9:
        return 0x19;
    case Key::Up:
        return 0x7E;
    case Key::Down:
        return 0x7D;
    case Key::Left:
        return 0x7B;
    case Key::Right:
        return 0x7C;
    case Key::Escape:
        return 0x35;
    case Key::Enter:
        return 0x24;
    case Key::Backspace:
        return 0x33;
    case Key::Delete:
        return 0x75;
    case Key::Home:
        return 0x73;
    case Key::End:
        return 0x77;
    case Key::Tab:
        return 0x30;
    case Key::Space:
        return 0x31;
    case Key::LeftShift:
        return 0x38;
    case Key::RightShift:
        return 0x3C;
    case Key::LeftCtrl:
        return 0x3B;
    case Key::RightCtrl:
        return 0x3E;
    case Key::LeftAlt:
        return 0x3A;
    case Key::RightAlt:
        return 0x3D;
    case Key::F1:
        return 0x7A;
    case Key::F2:
        return 0x78;
    case Key::F3:
        return 0x63;
    case Key::F4:
        return 0x76;
    case Key::F5:
        return 0x60;
    case Key::F6:
        return 0x61;
    case Key::F7:
        return 0x62;
    case Key::F8:
        return 0x64;
    case Key::F9:
        return 0x65;
    case Key::F10:
        return 0x6D;
    case Key::F11:
        return 0x67;
    case Key::F12:
        return 0x6F;
    default:
        return -1;
    }
}

inline bool initKeyState() {
    // CGEventSourceKeyState requires the host terminal app to be granted
    // Accessibility permission. AXIsProcessTrusted reflects that grant.
    if (!AXIsProcessTrusted()) {
        std::fprintf(stderr,
                     "tge: key-down state requires Accessibility permission.\n"
                     "Grant it to your terminal app in System Settings > Privacy & Security > Accessibility.\n");
        return false;
    }
    return true;
}
inline void shutdownKeyState() {}

inline bool isKeyDown(Key k) {
    int code = keyToNative(k);
    if (code < 0) return false;
    return CGEventSourceKeyState(kCGEventSourceStateHIDSystemState, static_cast<CGKeyCode>(code));
}

#else // Linux (evdev)

inline int keyToNative(Key k) {
    switch (k) {
    case Key::A:
        return KEY_A;
    case Key::B:
        return KEY_B;
    case Key::C:
        return KEY_C;
    case Key::D:
        return KEY_D;
    case Key::E:
        return KEY_E;
    case Key::F:
        return KEY_F;
    case Key::G:
        return KEY_G;
    case Key::H:
        return KEY_H;
    case Key::I:
        return KEY_I;
    case Key::J:
        return KEY_J;
    case Key::K:
        return KEY_K;
    case Key::L:
        return KEY_L;
    case Key::M:
        return KEY_M;
    case Key::N:
        return KEY_N;
    case Key::O:
        return KEY_O;
    case Key::P:
        return KEY_P;
    case Key::Q:
        return KEY_Q;
    case Key::R:
        return KEY_R;
    case Key::S:
        return KEY_S;
    case Key::T:
        return KEY_T;
    case Key::U:
        return KEY_U;
    case Key::V:
        return KEY_V;
    case Key::W:
        return KEY_W;
    case Key::X:
        return KEY_X;
    case Key::Y:
        return KEY_Y;
    case Key::Z:
        return KEY_Z;
    case Key::Num0:
        return KEY_0;
    case Key::Num1:
        return KEY_1;
    case Key::Num2:
        return KEY_2;
    case Key::Num3:
        return KEY_3;
    case Key::Num4:
        return KEY_4;
    case Key::Num5:
        return KEY_5;
    case Key::Num6:
        return KEY_6;
    case Key::Num7:
        return KEY_7;
    case Key::Num8:
        return KEY_8;
    case Key::Num9:
        return KEY_9;
    case Key::Up:
        return KEY_UP;
    case Key::Down:
        return KEY_DOWN;
    case Key::Left:
        return KEY_LEFT;
    case Key::Right:
        return KEY_RIGHT;
    case Key::Escape:
        return KEY_ESC;
    case Key::Enter:
        return KEY_ENTER;
    case Key::Backspace:
        return KEY_BACKSPACE;
    case Key::Delete:
        return KEY_DELETE;
    case Key::Home:
        return KEY_HOME;
    case Key::End:
        return KEY_END;
    case Key::Tab:
        return KEY_TAB;
    case Key::Space:
        return KEY_SPACE;
    case Key::LeftShift:
        return KEY_LEFTSHIFT;
    case Key::RightShift:
        return KEY_RIGHTSHIFT;
    case Key::LeftCtrl:
        return KEY_LEFTCTRL;
    case Key::RightCtrl:
        return KEY_RIGHTCTRL;
    case Key::LeftAlt:
        return KEY_LEFTALT;
    case Key::RightAlt:
        return KEY_RIGHTALT;
    case Key::F1:
        return KEY_F1;
    case Key::F2:
        return KEY_F2;
    case Key::F3:
        return KEY_F3;
    case Key::F4:
        return KEY_F4;
    case Key::F5:
        return KEY_F5;
    case Key::F6:
        return KEY_F6;
    case Key::F7:
        return KEY_F7;
    case Key::F8:
        return KEY_F8;
    case Key::F9:
        return KEY_F9;
    case Key::F10:
        return KEY_F10;
    case Key::F11:
        return KEY_F11;
    case Key::F12:
        return KEY_F12;
    default:
        return -1;
    }
}

inline std::vector<int>& keyboardFds() {
    static std::vector<int> fds;
    return fds;
}

inline bool bitSet(const unsigned long* bits, int code) {
    constexpr int W = 8 * sizeof(unsigned long);
    return (bits[code / W] & (1UL << (code % W))) != 0;
}

// A device qualifies as a keyboard if it advertises EV_KEY and at least KEY_A
// (filters out mice, gamepads, and power-button-only devices).
inline bool isKeyboardDevice(int fd) {
    constexpr int W = 8 * sizeof(unsigned long);
    unsigned long evbits[(EV_MAX + W) / W] = {0};
    if (ioctl(fd, EVIOCGBIT(0, sizeof(evbits)), evbits) < 0) return false;
    if (!bitSet(evbits, EV_KEY)) return false;

    unsigned long keybits[(KEY_MAX + W) / W] = {0};
    if (ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keybits)), keybits) < 0) return false;
    return bitSet(keybits, KEY_A);
}

inline bool initKeyState() {
    auto& fds = keyboardFds();
    DIR* dir = opendir("/dev/input");
    if (!dir) {
        std::fprintf(stderr, "tge: cannot open /dev/input: %s\n", std::strerror(errno));
        return false;
    }

    bool sawPermissionDenied = false;
    while (auto* ent = readdir(dir)) {
        if (std::strncmp(ent->d_name, "event", 5) != 0) continue;
        char path[300];
        std::snprintf(path, sizeof(path), "/dev/input/%s", ent->d_name);
        int fd = open(path, O_RDONLY | O_NONBLOCK | O_CLOEXEC);
        if (fd < 0) {
            if (errno == EACCES) sawPermissionDenied = true;
            continue;
        }
        if (isKeyboardDevice(fd)) {
            fds.push_back(fd);
        } else {
            close(fd);
        }
    }
    closedir(dir);

    if (fds.empty()) {
        if (sawPermissionDenied) {
            std::fprintf(stderr, "tge: no readable keyboard devices in /dev/input.\n"
                                 "Add your user to the 'input' group:\n"
                                 "    sudo usermod -aG input $USER\n"
                                 "Then log out and back in.\n");
        } else {
            std::fprintf(stderr, "tge: no keyboard devices found in /dev/input.\n");
        }
        return false;
    }
    return true;
}

inline void shutdownKeyState() {
    for (int fd : keyboardFds()) close(fd);
    keyboardFds().clear();
}

inline bool isKeyDown(Key k) {
    int code = keyToNative(k);
    if (code < 0) return false;

    constexpr int W = 8 * sizeof(unsigned long);
    unsigned long state[(KEY_MAX + W) / W] = {0};
    for (int fd : keyboardFds()) {
        if (ioctl(fd, EVIOCGKEY(sizeof(state)), state) < 0) continue;
        if (bitSet(state, code)) return true;
    }
    return false;
}

#endif

} // namespace platform
} // namespace tge
