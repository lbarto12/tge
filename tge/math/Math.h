#pragma once
// IWYU pragma: private, include <tge/graphics.h>

#include <algorithm>
namespace tge {
class Math {
public:
    Math() = delete;

    template <typename T> static T Clamp(T lo, T hi, T val) { return std::max(lo, std::min(hi, val)); };
};
} // namespace tge
