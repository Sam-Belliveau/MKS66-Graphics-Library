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

#include "TypeNames.hpp"
#include "Vector3D.hpp"

namespace SPGL 
{
    template<typename T>
    class Vec4 
    {
    public: // Variables
        T x, y, z, w;

    public: // Constructors
        constexpr Vec4() : x(0), y(0), z(0), w(0) {}
        constexpr Vec4(const T x, const T y, const T z, const T w = T(1.0)) : x(x), y(y), z(z), w(w) {} 
        constexpr Vec4(const std::array<T, 4>& arr) : x(arr[0]), y(arr[1]), z(arr[2]), w(arr[3]) {} 

        // Copy Constructors
        constexpr Vec4(const Vec4 &in) = default;
        constexpr Vec4& operator=(const Vec4 &in) = default;

        constexpr Vec4(const Vec3<T> &in) 
            : x(in.x), y(in.y), z(in.z), w(T(1.0)) {}

        // Custom Constructors
        template<class iT>
        constexpr Vec4(const Vec4<iT> &in)
            : x{static_cast<T>(in.x)}
            , y{static_cast<T>(in.y)}
            , z{static_cast<T>(in.z)}
            , w{static_cast<T>(in.w)} {}

    public: // Functions
        constexpr Vec3<T> getPosition() const
        { return Vec3<T>(x, y, z) / w; }

        T& operator[](Size i)
        {
            switch(i)
            {
                case 0: return x;
                case 1: return y;
                case 2: return z;
                case 3: return w;
                default: return w;
            }
        }

        const T& operator[](Size i) const
        {
            switch(i)
            {
                case 0: return x;
                case 1: return y;
                case 2: return z;
                case 3: return w;
                default: return w;
            }
        }

    public: // Operators
        constexpr Vec4& operator*=(const T rhs) { x *= rhs; y *= rhs; z *= rhs; w *= rhs; return *this; }
        constexpr Vec4& operator/=(const T rhs) { x /= rhs; y /= rhs; z /= rhs; w *= rhs; return *this; }
        constexpr Vec4  operator-() const { return Vec4(-x, -y, -z, -w); }

        constexpr friend Vec4 operator*(Vec4 lhs, const T rhs) { return lhs *= rhs; }
        constexpr friend Vec4 operator*(const T lhs, Vec4 rhs) { return rhs *= lhs; }
        constexpr friend Vec4 operator/(Vec4 lhs, const T rhs) { return lhs /= rhs; }

    public: // Print Support
        friend std::ostream& operator<<(std::ostream& file, const Vec4& vec)
        { return file << "[ " << vec.x << '\t' << vec.y << '\t' << vec.z << '\t' << vec.w << " ]"; }
    };

    using Vec4i = Vec4<Int32>;
    using Vec4u = Vec4<UInt32>;

    using Vec4s = Vec4<Size>;

    using Vec4f = Vec4<Float32>;
    using Vec4d = Vec4<Float64>;
}

namespace SPGL
{
    template<class T>
    using EdgeList = std::vector<Vec4<T>>;

    template<class T>
    std::ostream& operator<<(std::ostream& file, const EdgeList<T>& list)
    {
        for(Size i = 0; i < 4; ++i)
        {
            file << "| ";
            for(const Vec4<T>& edge : list)
            { file << edge[i] << "\t| "; }
            file << '\n';
        }

        return file;
    }
}