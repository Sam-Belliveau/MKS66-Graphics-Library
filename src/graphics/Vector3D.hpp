#pragma once

/**
 * Copyright (c) 2022 Sam Belliveau
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 */

#include <cmath>
#include <array>

namespace SPGL 
{
    template<typename T>
    class Vec3 
    {
    public: // Variables
        T x, y, z;

    public: // Constructors
        constexpr Vec3() : x(0), y(0), z(0) {}
        constexpr Vec3(T x, T y, T z) : x(x), y(y), z(z) {} 

        // Copy Constructors
        constexpr Vec3(const Vec3 &in) = default;
        constexpr Vec3& operator=(const Vec3 &in) = default;

        // Custom Constructors
        template<class iT>
        constexpr Vec2(const Vec3<iT> &in)
            : x{static_cast<T>(in.x)}
            , y{static_cast<T>(in.y)}
            , z{static_cast<T>(in.z)} {}

        // Casting
        constexpr operator std::array<T, 3>() const 
        { return std::array<T, 3>({x, y, z}); }

    public: // Functions
        constexpr T mag() const 
        { return std::sqrt(x*x + y*y + z*z); }

        constexpr T dot(const Vec3& rhs) const 
        { return x * rhs.x + y * rhs.y + z * rhs.z; }

        constexpr Vec3 normalized() const 
        { return *this / mag(); }

        constexpr Vec3 abs() const 
        { return Vec3(std::abs(x), std::abs(y), std::abs(z)); }

    public: // Operators

        constexpr Vec3& operator+=(const Vec3& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
        constexpr Vec3& operator-=(const Vec3& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
        constexpr Vec3& operator*=(const Vec3& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
        constexpr Vec3& operator/=(const Vec3& rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }
        constexpr Vec3& operator*=(const T rhs)     { x *= rhs;   y *= rhs;   z *= rhs;   return *this; }
        constexpr Vec3& operator/=(const T rhs)     { x /= rhs;   y /= rhs;   z /= rhs;   return *this; }
        constexpr Vec3  operator-() const { return Vec3(-x, -y, -z); }

        constexpr friend Vec3 operator+(Vec3 lhs, const Vec3& rhs) { return lhs += rhs; }
        constexpr friend Vec3 operator-(Vec3 lhs, const Vec3& rhs) { return lhs -= rhs; }
        constexpr friend Vec3 operator*(Vec3 lhs, const Vec3& rhs) { return lhs *= rhs; }
        constexpr friend Vec3 operator/(Vec3 lhs, const Vec3& rhs) { return lhs /= rhs; }
        constexpr friend Vec3 operator*(Vec3 lhs, const T rhs) { return lhs *= rhs; }
        constexpr friend Vec3 operator*(const T lhs, Vec3 rhs) { return rhs *= lhs; }
        constexpr friend Vec3 operator/(Vec3 lhs, const T rhs) { return lhs /= rhs; }
    };

    using Vec3i = Vec3<Int32>;
    using Vec3u = Vec3<UInt32>;

    using Vec3s = Vec3<Size>;

    using Vec3f = Vec3<Float32>;
    using Vec3d = Vec3<Float64>;
}