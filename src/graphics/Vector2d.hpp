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

namespace SPGL
{
    template<class T = Int32>
    struct Vector2D
    {
    public: /* Methods */
        // Default Constructor
        constexpr Vector2D() : x{0}, y{0} {}

        // Copy Constructors
        constexpr Vector2D(const Vector2D &in) = default;
        constexpr Vector2D& operator=(const Vector2D &in) = default;

        // Custom Constructors
        template<class iT>
        constexpr Vector2D(const Vector2D<iT> &in)
            : x{static_cast<T>(in.x)}, y{static_cast<T>(in.y)} {}

        template<class iT>
        constexpr Vector2D& operator=(const Vector2D<iT> &in)
        {
            x = static_cast<T>(in.x);
            y = static_cast<T>(in.y);
            return *this;
        }

        constexpr Vector2D(const T ix, const T iy) : x{ix}, y{iy} {}

    public: /* Variables */
        T x, y;
    };

    using Vector2i = Vector2D<Int32>;
    using Vector2u = Vector2D<UInt32>;

    using Vector2s = Vector2D<Size>;

    using Vector2f = Vector2D<Float32>;
    using Vector2d = Vector2D<Float>;
    using Vector2l = Vector2D<Float80>;
}

namespace SPGL // Operator Overloads
{
    // ADD //
    template<class T>
    Vector2D<T>& operator+=(Vector2D<T>& a, const Vector2D<T> b)
    { a.x += b.x; a.y += b.y; return a; }

    template<class T>
    Vector2D<T>  operator+ (Vector2D<T> a, const Vector2D<T> b)
    { a += b; return a; }

    // SUB //
    template<class T>
    Vector2D<T>& operator-=(Vector2D<T>& a, const Vector2D<T> b)
    { a.x -= b.x; a.y -= b.y; return a; }

    template<class T>
    Vector2D<T>  operator- (Vector2D<T> a, const Vector2D<T> b)
    { a -= b; return a; }

    // MUL //
    template<class T>
    Vector2D<T>& operator*=(Vector2D<T>& a, const Vector2D<T> b)
    { a.x *= b.x; a.y *= b.y; return a; }

    template<class T>
    Vector2D<T>  operator* (Vector2D<T> a, const Vector2D<T> b)
    { a *= b; return a; }

    // DIV //
    template<class T>
    Vector2D<T>& operator/=(Vector2D<T>& a, const Vector2D<T> b)
    { a.x /= b.x; a.y /= b.y; return a; }

    template<class T>
    Vector2D<T>  operator/ (Vector2D<T> a, const Vector2D<T> b)
    { a /= b; return a; }

    // EQUAL //
    template<class T>
    bool operator==(const Vector2D<T> a, const Vector2D<T> b)
    { return a.x == b.x && a.y == b.y; }

    template<class T>
    bool operator!=(const Vector2D<T> a, const Vector2D<T> b)
    { return !(a == b); }

    // COMPARE //
    template<class T>
    bool operator< (const Vector2D<T> a, const Vector2D<T> b)
    { return (a.x*a.x + a.y*a.y) < (b.x*b.x + b.y*b.y); }

    template<class T>
    bool operator>=(const Vector2D<T> a, const Vector2D<T> b)
    { return !(a < b); }

    template<class T>
    bool operator> (const Vector2D<T> a, const Vector2D<T> b)
    { return b < a; }

    template<class T>
    bool operator<=(const Vector2D<T> a, const Vector2D<T> b)
    { return !(a > b); }
}