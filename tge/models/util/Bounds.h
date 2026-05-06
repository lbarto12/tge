#pragma once

namespace tge {
template <typename T> struct Rect {
    T x;
    T y;
    T width;
    T height;

    bool Intersects(const Rect<T>& other) const {
        return x < other.x + other.width && x + width > other.x && y < other.y + other.height && y + height > other.y;
    }

    bool Contains(const Rect<T>& other) const {
        return x <= other.x && y <= other.y && x + width >= other.x + other.width &&
               y + height >= other.y + other.height;
    }

    bool Covers(const Rect<T>* other) const {
        return other != nullptr && x <= other->x && y <= other->y && x + width >= other->x + other->width &&
               y + height >= other->y + other->height;
    }
};

typedef Rect<float> FloatRect;
typedef Rect<double> DoubleRect;
typedef Rect<int> IntRect;
typedef Rect<unsigned int> UIntRect;
typedef Rect<long> LongRect;
typedef Rect<unsigned long> ULongRect;

} // namespace tge
