#pragma once
// IWYU pragma: private, include <tge/graphics.h>

#include <cmath>

namespace tge {
template <typename T> struct Vector2 {
    T x, y;
    bool operator==(const Vector2<T>& other) const { return x == other.x && y == other.y; }
    bool operator==(T s) const { return x == s && y == s; }
    bool operator!=(const Vector2<T>& other) const { return !(*this == other); }
    bool operator!=(T s) const { return !(*this == s); }
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
    bool operator==(const Vector3<T>& other) const { return x == other.x && y == other.y && z == other.z; }
    bool operator==(T s) const { return x == s && y == s && z == s; }
    bool operator!=(const Vector3<T>& other) const { return !(*this == other); }
    bool operator!=(T s) const { return !(*this == s); }

    /**
     * Drop the z component, returning the (x, y) pair as a `Vector2<T>`.
     */
    Vector2<T> XY() const { return {x, y}; }
    Vector3<T> operator+(const Vector3<T>& other) const { return {x + other.x, y + other.y, z + other.z}; }
    Vector3<T> operator-(const Vector3<T>& other) const { return {x - other.x, y - other.y, z - other.z}; }
    Vector3<T> operator*(const Vector3<T>& other) const { return {x * other.x, y * other.y, z * other.z}; }
    Vector3<T> operator/(const Vector3<T>& other) const { return {x / other.x, y / other.y, z / other.z}; }
    Vector3<T> operator%(const Vector3<T>& other) const { return {x % other.x, y % other.y, z % other.z}; }
    Vector3<T> operator+(T s) const { return {x + s, y + s, z + s}; }
    Vector3<T> operator-(T s) const { return {x - s, y - s, z - s}; }
    Vector3<T> operator*(T s) const { return {x * s, y * s, z * s}; }
    Vector3<T> operator/(T s) const { return {x / s, y / s, z / s}; }
    Vector3<T> operator%(T s) const { return {x % s, y % s, z % s}; }

    static Vector3<T> Cross(const Vector3<T>& a, const Vector3<T>& b) {
        return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
    }
    static T Dot(const Vector3<T>& a, const Vector3<T>& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

    /**
     * Rotate this vector around a unit-length axis by `degrees` (right-hand rule).
     * Uses Rodrigues' formula; the axis must be normalized.
     *
     * @param axis unit-length rotation axis
     * @param degrees rotation in degrees
     * @return the rotated vector
     */
    Vector3<T> Rotate(const Vector3<T>& axis, T degrees) const {
        constexpr T DEG2RAD = T(3.14159265358979323846L / 180.0L);
        T rad = degrees * DEG2RAD;
        T c = std::cos(rad), s = std::sin(rad);
        return *this * c + Cross(axis, *this) * s + axis * (Dot(axis, *this) * (T(1) - c));
    }
};

typedef Vector3<float> Vector3f;
typedef Vector3<int> Vector3i;
typedef Vector3<unsigned int> Vector3u;
} // namespace tge
