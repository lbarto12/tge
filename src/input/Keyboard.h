#include <optional>

#include "./Platform.h"

namespace gte {
enum class Key { None = 0, Character, Up, Down, Left, Right, Escape, Enter, Backspace, Delete, Home, End, Tab };

struct KeyEvent {
    Key key = Key::None;
    char chr = '\0';
    bool Is(const char c) { return key == Key::Character && chr == c; }
    bool Is(Key k) { return key == k; }
};

struct Keyboard {
    static KeyEvent GetKeyPress() {
        std::optional<char> key = platform::getKey();

        if (!key) return {Key::None};

        // simple single-byte keys
        if (*key == '\r' || *key == '\n') return {Key::Enter};
        if (*key == '\t') return {Key::Tab};
        if (*key == 127 || *key == 8) return {Key::Backspace};

        // escape seuqences
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
                    // Delete = \x1b[3~
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
};

} // namespace gte
