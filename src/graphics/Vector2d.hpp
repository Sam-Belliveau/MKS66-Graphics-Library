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

#include "TypeNames.hpp"

#include <iostream>

#include <cmath>

namespace SPGL
{
    template<class T>
    struct Vec2
    {
    public: /* Variables */
        T x, y;

    public: /* Methods */
        // Default Constructor
        constexpr Vec2() : x{0}, y{0} {}
        constexpr Vec2(const T ix, const T iy) : x{ix}, y{iy} {}
        constexpr Vec2(const std::array<T, 3>& arr) : x(arr[0]), y(arr[1]) {} 

        // Copy Constructors
        constexpr Vec2(const Vec2 &in) = default;
        constexpr Vec2& operator=(const Vec2 &in) = default;

        // Custom Constructors
        template<class iT>
        constexpr Vec2(const Vec2<iT> &in)
            : x{static_cast<T>(in.x)}
            , y{static_cast<T>(in.y)} {}
    
    public: // Functions
        constexpr T mag() const 
        { return std::sqrt(x*x + y*y); }

        constexpr T dot(const Vec2& rhs) const 
        { return x * rhs.x + y * rhs.y; }

        constexpr Vec2 normalized() const 
        { return *this / mag(); }

        constexpr Vec2 abs() const 
        { return Vec2(std::abs(x), std::abs(y)); }

    public: // Operators
        constexpr Vec2& operator+=(const Vec2& rhs) { x += rhs.x; y += rhs.y; return *this; }
        constexpr Vec2& operator-=(const Vec2& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
        constexpr Vec2& operator*=(const Vec2& rhs) { x *= rhs.x; y *= rhs.y; return *this; }
        constexpr Vec2& operator/=(const Vec2& rhs) { x /= rhs.x; y /= rhs.y; return *this; }
        constexpr Vec2& operator*=(const T rhs)     { x *= rhs;   y *= rhs;   return *this; }
        constexpr Vec2& operator/=(const T rhs)     { x /= rhs;   y /= rhs;   return *this; }
        constexpr Vec2  operator-() const { return Vec2(-x, -y); }

        constexpr friend Vec2 operator+(Vec2 lhs, const Vec2& rhs) { return lhs += rhs; }
        constexpr friend Vec2 operator-(Vec2 lhs, const Vec2& rhs) { return lhs -= rhs; }
        constexpr friend Vec2 operator*(Vec2 lhs, const Vec2& rhs) { return lhs *= rhs; }
        constexpr friend Vec2 operator/(Vec2 lhs, const Vec2& rhs) { return lhs /= rhs; }
        constexpr friend Vec2 operator*(Vec2 lhs, const T rhs) { return lhs *= rhs; }
        constexpr friend Vec2 operator*(const T lhs, Vec2 rhs) { return rhs *= lhs; }
        constexpr friend Vec2 operator/(Vec2 lhs, const T rhs) { return lhs /= rhs; }

    public: // Print Support
        friend std::ostream& operator<<(std::ostream& file, const Vec2& vec)
        { return file << "[ " << vec.x << '\t' << vec.y << " ]"; }

        friend std::istream& operator>>(std::istream& file, Vec2& vec)
        { return file >> vec.x >> vec.y; }

    };

    using Vec2i = Vec2<Int32>;
    using Vec2u = Vec2<UInt32>;

    using Vec2s = Vec2<Size>;

    using Vec2f = Vec2<Float32>;
    using Vec2d = Vec2<Float64>;
}