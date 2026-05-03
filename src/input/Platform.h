
#include <optional>

#ifdef _WIN32
#include <conio.h>
#else
#include <sys/select.h>
#include <unistd.h>
#endif // _WIN32

namespace gte::platform {
inline bool keyhit() {
#ifdef _WIN32
    return _kbhit() != 0;
#else
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    struct timeval tv = {0, 0};
    return select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) > 0;
#endif // _WIN32
}

inline std::optional<char> getKey() {
    if (!keyhit()) return std::nullopt;

#ifdef _WIN32
    return static_cast<char>(_getch());
#else
    char c;
    if (read(STDIN_FILENO, &c, 1) == 1) return c;
    return std::nullopt;
#endif // _WIN32
}

static std::optional<char> readByteTimeout() {
#ifdef _WIN32
    // On Windows, _getch already handles extended keys via two-call protocol
    if (_kbhit()) return static_cast<char>(_getch());
    return std::nullopt;
#else
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    // Short timeout to catch the rest of an escape sequence
    struct timeval tv = {0, 50'000}; // 50 ms
    if (select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) > 0) {
        char c;
        if (read(STDIN_FILENO, &c, 1) == 1) return c;
    }
    return std::nullopt;
#endif
}

} // namespace gte::platform
