#pragma once
#include <cctype>
#include <optional>

#include "./Platform.h"

namespace tge {

struct KeyEvent {
    Key key = Key::None;
    char chr = '\0';
    bool Is(const char c) { return key == Key::Character && chr == c; }
    bool Is(Key k) { return key == k; }
};

struct Keyboard {
    static bool Init() { return platform::initKeyState(); }
    static void Shutdown() { platform::shutdownKeyState(); }

    static KeyEvent GetKeyPressed() {
        std::optional<char> key = platform::getKey();

        if (!key) return {Key::None};

        if (*key == '\r' || *key == '\n') return {Key::Enter};
        if (*key == '\t') return {Key::Tab};
        if (*key == 127 || *key == 8) return {Key::Backspace};

        if (*key == '\x1b') {
            auto second = platform::readByteTimeout();
            if (!second) return {Key::Escape};

            if (*second == '[') {
                auto third = platform::readByteTimeout();
                if (!third) return {Key::Escape};

                switch (*third) {
                case 'A':
                    return {Key::Up};
                case 'B':
                    return {Key::Down};
                case 'C':
                    return {Key::Right};
                case 'D':
                    return {Key::Left};
                case 'H':
                    return {Key::Home};
                case 'F':
                    return {Key::End};
                case '3': {
                    auto tilde = platform::readByteTimeout();
                    if (tilde && *tilde == '~') return {Key::Delete};
                    break;
                }
                }
            }

            return {Key::Escape};
        }
        return {Key::Character, *key};
    }

    static bool GetKeyDown(Key k) { return platform::isKeyDown(k); }

    static bool GetKeyDown(char c) {
        char up = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        if (up >= 'A' && up <= 'Z') {
            return platform::isKeyDown(static_cast<Key>(static_cast<int>(Key::A) + (up - 'A')));
        }
        if (c >= '0' && c <= '9') {
            return platform::isKeyDown(static_cast<Key>(static_cast<int>(Key::Num0) + (c - '0')));
        }
        if (c == ' ') return platform::isKeyDown(Key::Space);
        if (c == '\t') return platform::isKeyDown(Key::Tab);
        if (c == '\r' || c == '\n') return platform::isKeyDown(Key::Enter);
        return false;
    }
};

} // namespace tge
