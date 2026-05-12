#pragma once

#if defined(_WIN32)
#define TGE_PLATFORM_WINDOWS 1
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#define TGE_PLATFORM_POSIX 1
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#endif

namespace tge {

struct TerminalSize {
    int cols;
    int rows;
};

namespace platform {

#if defined(TGE_PLATFORM_WINDOWS)

struct ModeState {
    DWORD inMode = 0;
    DWORD outMode = 0;
    bool saved = false;
};

inline ModeState& savedMode() {
    static ModeState s;
    return s;
}

inline TerminalSize getTerminalSize() {
    CONSOLE_SCREEN_BUFFER_INFO info;
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    if (out == INVALID_HANDLE_VALUE || !GetConsoleScreenBufferInfo(out, &info)) return {80, 24};
    return {
        info.srWindow.Right - info.srWindow.Left + 1,
        info.srWindow.Bottom - info.srWindow.Top + 1,
    };
}

// Opt the legacy console into ANSI escape processing. No-op on POSIX.
inline bool enableAnsi() {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    if (out == INVALID_HANDLE_VALUE) return false;
    DWORD mode = 0;
    if (!GetConsoleMode(out, &mode)) return false;
    return SetConsoleMode(out, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
}

inline bool enableRawMode() {
    HANDLE in = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    if (in == INVALID_HANDLE_VALUE || out == INVALID_HANDLE_VALUE) return false;

    auto& s = savedMode();
    if (!GetConsoleMode(in, &s.inMode) || !GetConsoleMode(out, &s.outMode)) return false;
    s.saved = true;

    DWORD newIn = s.inMode;
    newIn &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT);
    newIn |= ENABLE_VIRTUAL_TERMINAL_INPUT;

    DWORD newOut = s.outMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    return SetConsoleMode(in, newIn) != 0 && SetConsoleMode(out, newOut) != 0;
}

inline void disableRawMode() {
    auto& s = savedMode();
    if (!s.saved) return;
    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), s.inMode);
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), s.outMode);
    s.saved = false;
}

inline bool isTerminalFocused() {
    HWND console = GetConsoleWindow();
    HWND fg = GetForegroundWindow();
    return console == fg;
}

#else // POSIX

struct ModeState {
    struct termios original;
    bool saved = false;
};

inline ModeState& savedMode() {
    static ModeState s;
    return s;
}

inline TerminalSize getTerminalSize() {
    struct winsize ws{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) return {80, 24};
    return {static_cast<int>(ws.ws_col), static_cast<int>(ws.ws_row)};
}

inline bool enableAnsi() { return true; }

inline bool enableRawMode() {
    auto& s = savedMode();
    if (tcgetattr(STDIN_FILENO, &s.original) == -1) return false;
    s.saved = true;

    struct termios raw = s.original;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= CS8;
    raw.c_cc[VMIN] = 0;  // non-blocking read
    raw.c_cc[VTIME] = 0; // no read timeout

    return tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == 0;
}

inline void disableRawMode() {
    auto& s = savedMode();
    if (!s.saved) return;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &s.original);
    s.saved = false;
}

bool isTerminalFocused();

#endif

} // namespace platform
} // namespace tge
