#pragma once
// IWYU pragma: private, include <tge/game.h>

#include <algorithm>
#include <cstdlib>
#include <random>
#include <vector>

#include "../models/util/Vector2.h"
namespace tge {
struct Random {
    Random() = delete;

    static int Int(int lo, int hi) {
        std::uniform_int_distribution<> distr(lo, hi);
        return distr(gen);
    }

    static tge::Vector2i Vector2(int lox, int hix, int loy, int hiy) {
        std::uniform_int_distribution<> dx(lox, hix);
        std::uniform_int_distribution<> dy(loy, hiy);

        return tge::Vector2i{dx(gen), dy(gen)};
    }

    template <typename T> static void Shuffle(std::vector<T>& v) { std::shuffle(v.begin(), v.end()); }

    template <typename T> static T Choice(const std::vector<T>& v) {
        std::uniform_int_distribution<> distr(0, v.size() - 1);
        std::string choice = v[distr(gen)];
        return choice;
    }

private:
    static inline std::mt19937 gen{std::random_device{}()};
};
} // namespace tge
