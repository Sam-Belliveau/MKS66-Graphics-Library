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
    template<class T>
    struct Vec2
    {
    public: /* Methods */
        // Default Constructor
        constexpr Vec2() : x{0}, y{0} {}

        // Copy Constructors
        constexpr Vec2(const Vec2 &in) = default;
        constexpr Vec2& operator=(const Vec2 &in) = default;

        // Custom Constructors
        template<class iT>
        constexpr Vec2(const Vec2<iT> &in)
            : x{static_cast<T>(in.x)}
            , y{static_cast<T>(in.y)} {}

        template<class iT>
        constexpr Vec2& operator=(const Vec2<iT> &in)
        {
            x = static_cast<T>(in.x);
            y = static_cast<T>(in.y);
            return *this;
        }

        constexpr Vec2(const T ix, const T iy) : x{ix}, y{iy} {}

    public: /* Variables */
        T x, y;
    };

    using Vec2i = Vec2<Int32>;
    using Vec2u = Vec2<UInt32>;

    using Vec2s = Vec2<Size>;

    using Vec2f = Vec2<Float32>;
    using Vec2d = Vec2<Float64>;
}

namespace SPGL // Operator Overloads
{
    // ADD //
    template<class T>
    Vec2<T>& operator+=(Vec2<T>& a, const Vec2<T> b)
    { a.x += b.x; a.y += b.y; return a; }

    template<class T>
    Vec2<T>  operator+ (Vec2<T> a, const Vec2<T> b)
    { a += b; return a; }

    // SUB //
    template<class T>
    Vec2<T>& operator-=(Vec2<T>& a, const Vec2<T> b)
    { a.x -= b.x; a.y -= b.y; return a; }

    template<class T>
    Vec2<T>  operator- (Vec2<T> a, const Vec2<T> b)
    { a -= b; return a; }

    // MUL //
    template<class T>
    Vec2<T>& operator*=(Vec2<T>& a, const Vec2<T> b)
    { a.x *= b.x; a.y *= b.y; return a; }

    template<class T>
    Vec2<T>  operator* (Vec2<T> a, const Vec2<T> b)
    { a *= b; return a; }

    // DIV //
    template<class T>
    Vec2<T>& operator/=(Vec2<T>& a, const Vec2<T> b)
    { a.x /= b.x; a.y /= b.y; return a; }

    template<class T>
    Vec2<T>  operator/ (Vec2<T> a, const Vec2<T> b)
    { a /= b; return a; }

    // EQUAL //
    template<class T>
    bool operator==(const Vec2<T> a, const Vec2<T> b)
    { return a.x == b.x && a.y == b.y; }

    template<class T>
    bool operator!=(const Vec2<T> a, const Vec2<T> b)
    { return !(a == b); }

    // COMPARE //
    template<class T>
    bool operator< (const Vec2<T> a, const Vec2<T> b)
    { return (a.x*a.x + a.y*a.y) < (b.x*b.x + b.y*b.y); }

    template<class T>
    bool operator>=(const Vec2<T> a, const Vec2<T> b)
    { return !(a < b); }

    template<class T>
    bool operator> (const Vec2<T> a, const Vec2<T> b)
    { return b < a; }

    template<class T>
    bool operator<=(const Vec2<T> a, const Vec2<T> b)
    { return !(a > b); }
}