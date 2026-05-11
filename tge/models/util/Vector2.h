#pragma once
// IWYU pragma: private, include <tge/graphics.h>

namespace tge {
template <typename T> struct Vector2 {
    T x, y;
    bool operator==(const Vector2<T>& other) const { return x == other.x && y == other.y; }
    bool operator==(T s) const { return x == s && y == s; }
    Vector2<T> operator+(const Vector2<T>& other) const { return {x + other.x, y + other.y}; }
    Vector2<T> operator-(const Vector2<T>& other) const { return {x - other.x, y - other.y}; }
    Vector2<T> operator*(const Vector2<T>& other) const { return {x * other.x, y * other.y}; }
    Vector2<T> operator/(const Vector2<T>& other) const { return {x / other.x, y / other.y}; }
    Vector2<T> operator%(const Vector2<T>& other) const { return {x % other.x, y % other.y}; }
    Vector2<T> operator+(T s) const { return {x + s, y + s}; }
    Vector2<T> operator-(T s) const { return {x - s, y - s}; }
    Vector2<T> operator*(T s) const { return {x * s, y * s}; }
    Vector2<T> operator/(T s) const { return {x / s, y / s}; }
    Vector2<T> operator%(T s) const { return {x % s, y % s}; }
};

typedef Vector2<float> Vector2f;
typedef Vector2<int> Vector2i;
typedef Vector2<unsigned int> Vector2u;

template <typename T> struct Vector3 {
    T x, y, z;
};
} // namespace tge
