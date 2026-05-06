#pragma once

#include <cstdint>
#include <string>

namespace tge {

enum class Color : uint8_t {
    Black = 0,
    Red = 1,
    Green = 2,
    Yellow = 3,
    Blue = 4,
    Magenta = 5,
    Cyan = 6,
    White = 7,
    BrightBlack = 8,
    BrightRed = 9,
    BrightGreen = 10,
    BrightYellow = 11,
    BrightBlue = 12,
    BrightMagenta = 13,
    BrightCyan = 14,
    BrightWhite = 15,
};

inline constexpr const char* RESET = "\033[0m";

inline std::string fg(uint8_t code) { return "\033[38;5;" + std::to_string(code) + "m"; }
inline std::string bg(uint8_t code) { return "\033[48;5;" + std::to_string(code) + "m"; }

inline std::string fg(Color c) { return fg(static_cast<uint8_t>(c)); }
inline std::string bg(Color c) { return bg(static_cast<uint8_t>(c)); }

} // namespace tge
