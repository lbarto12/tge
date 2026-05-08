#pragma once
// IWYU pragma: private, include <tge/graphics.h>

#include <algorithm>
namespace tge {
class Math {
public:
    Math() = delete;

    /**
     * Clamp a value between two values, inclusive.
     *
     * @param lo the lower bound
     * @param hi the upper bound
     * @return clamped value
     */
    template <typename T> static T Clamp(T lo, T hi, T val) { return std::max(lo, std::min(hi, val)); };
};
} // namespace tge
