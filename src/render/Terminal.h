#pragma once

#include <cstdio>

#include "Color.h"
#include "Platform.h"

namespace tge::render {

class Terminal {
public:
    static bool Init() { return platform::enableAnsi(); }

    static void Clear() { std::fputs("\033[2J\033[H", stdout); }
    static void Home() { std::fputs("\033[H", stdout); }

    static void MoveTo(int col, int row) { std::printf("\033[%d;%dH", row + 1, col + 1); }

    static void HideCursor() { std::fputs("\033[?25l", stdout); }
    static void ShowCursor() { std::fputs("\033[?25h", stdout); }

    static void SetForeground(Color c) { std::fputs(fg(c).c_str(), stdout); }
    static void SetBackground(Color c) { std::fputs(bg(c).c_str(), stdout); }
    static void SetForeground(uint8_t code) { std::fputs(fg(code).c_str(), stdout); }
    static void SetBackground(uint8_t code) { std::fputs(bg(code).c_str(), stdout); }
    static void ResetStyle() { std::fputs(RESET, stdout); }

    static TerminalSize Size() { return platform::getTerminalSize(); }

    static bool EnableRawMode() { return platform::enableRawMode(); }
    static void DisableRawMode() { platform::disableRawMode(); }

    static void Flush() { std::fflush(stdout); }
};

} // namespace tge::render
