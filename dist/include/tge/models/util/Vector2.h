#pragma once

namespace tge {
template <typename T> struct Vector2 {
    T x, y;
    bool operator==(const Vector2<T>& other) { return x == other.x && y == other.y; }
    Vector2<T> operator+(const Vector2<T>& other) { return {x + other.x, y + other.y}; }
    Vector2<T> operator-(const Vector2<T>& other) { return {x - other.x, y - other.y}; }
};

typedef Vector2<float> Vector2f;
typedef Vector2<int> Vector2i;
typedef Vector2<unsigned int> Vector2u;

template <typename T> struct Vector3 {
    T x, y, z;
};
} // namespace tge
