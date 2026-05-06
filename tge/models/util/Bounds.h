#pragma once

namespace tge {
template <typename T> struct Rect {
    T x;
    T y;
    T width;
    T height;
};

typedef Rect<float> FloatRect;
typedef Rect<double> DoubleRect;
typedef Rect<int> IntRect;
typedef Rect<unsigned int> UIntRect;
typedef Rect<long> LongRect;
typedef Rect<unsigned long> ULongRect;

} // namespace tge
