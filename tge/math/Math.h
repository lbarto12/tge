#pragma once
// IWYU pragma: private, include <tge/graphics.h>

#include <algorithm>
#include <utility>

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
    template <typename T> static T Clamp(T lo, T hi, T val) { return std::max(lo, std::min(hi, val)); }

    /**
     * Wrap a value around a range, inclusive.
     *
     * @param lo the lower bound
     * @param hi the upper bound
     * @return the wrapped value
     */
    template <typename T> static T Wrap(T lo, T hi, T val) {
        T range = hi - lo, v = val - lo;
        if (range == 0) return lo;
        return lo + (v % range + range) % range;
    }

    /**
     * Mathematical modulo.
     *
     * e.g. -1 mod 10 = 9.
     *
     *
     * @param a the number to modulo
     * @param m the modulo
     * @return a mod m
     */
    template <typename T> static T Mod(T a, T m) { return (a % m + m) % m; }

    /**
     * Return the quotient and remainder of a division operation.
     *
     * @param a the dividend
     * @param b the divisor
     */
    template <typename T> static std::pair<T, T> DivMod(T a, T b) {
        T q = a / b;
        T r = a % b;

        // Adjust if signs are different and there is a remainder
        if ((r != 0) && ((a < 0) ^ (b < 0))) {
            q = q - 1;
            r = r + b;
        }

        return {q, r};
    }
};
} // namespace tge
