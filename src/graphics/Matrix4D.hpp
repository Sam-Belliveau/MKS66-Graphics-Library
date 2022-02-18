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
#include "Vector4D.hpp"

#include <cmath>
#include <array>

namespace SPGL 
{
    template<typename T>
    class Mat4 {
    public: // Typedefs
        constexpr static Size width() { return 4; }
        constexpr static Size height() { return 4; }

        using Column = Vec4<T>;
        using Rows = std::array<Column, height()>;

    public: // Factories
        constexpr static Mat4 Identity() 
        {
            return Mat4(
                {T(1),T(0),T(0),T(0)},
                {T(0),T(1),T(0),T(0)},
                {T(0),T(0),T(1),T(0)},
                {T(0),T(0),T(0),T(1)}
            );
        }

        constexpr static Mat4 Pitch(const T& t) 
        {
            const T S = std::sin(t);
            const T C = std::cos(t);
            return Mat4{
                { +C, +0, +S, +0 },
                { +0, +1, +0, +0 },
                { -S, +0, +C, +0 },
                { +0, +0, +0, +1 }
            };
        }

        constexpr static Mat4 Yaw(const T& t) 
        {
            const T S = std::sin(t);
            const T C = std::cos(t);
            return Mat4{
                { +C, -S, +0, +0 },
                { +S, +C, +0, +0 },
                { +0, +0, +1, +0 },
                { +0, +0, +0, +1 }
            };
        }

        constexpr static Mat4 Roll(const T& t) 
        {
            const T S = std::sin(t);
            const T C = std::cos(t);
            return Mat4{
                { +1, +0, +0, +0 },
                { +0, +C, -S, +0 },
                { +0, +S, +C, +0 },
                { +0, +0, +0, +1 }
            };
        }
 

    private: // Variables
        Rows _data;

    public: // Constructors
        constexpr Mat4() : _data(Identity()._data) {}
        constexpr Mat4(const Rows& data) : _data{data} {}
        constexpr Mat4(const Column& a, const Column& b, const Column& c, const Column& d) 
            : _data{{a, b, c, d}} {}
        
        constexpr Mat4(const Mat4& other) = default;
        constexpr Mat4& operator=(const Mat4& other) = default;

    public: // Getters
        constexpr /***/ Column& operator[](std::size_t row) /***/ { return _data[row]; }
        constexpr const Column& operator[](std::size_t row) const { return _data[row]; }

        constexpr /***/ T& operator()(std::size_t row, std::size_t column) /***/ { return _data[row][column]; }
        constexpr const T& operator()(std::size_t row, std::size_t column) const { return _data[row][column]; }

    public: // Math Operators
        constexpr friend Mat4 operator+(Mat4 lhs, const Mat4& rhs) { return lhs += rhs; }
        constexpr Mat4& operator+=(const Mat4& rhs) 
        {
            for(Size y = 0; y < height(); ++y)
            for(Size x = 0; x < width(); ++x)
                _data[y][x] += rhs[y][x];

            return *this;
        }

        constexpr friend Mat4 operator-(Mat4 lhs, const Mat4& rhs) { return lhs -= rhs; }
        constexpr Mat4& operator-=(const Mat4& rhs) 
        {
            for(Size y = 0; y < height(); ++y)
            for(Size x = 0; x < width(); ++x)
                _data[y][x] -= rhs[y][x];

            return *this;
        }

        constexpr friend Mat4 operator*(Mat4 lhs, const T& rhs) { return lhs *= rhs; }
        constexpr friend Mat4 operator*(const T& lhs, Mat4 rhs) { return rhs *= lhs; }
        constexpr Mat4& operator*=(const T& rhs) 
        {
            for(auto& row : _data) row *= rhs;   
            return *this;
        }    
        
        constexpr friend Mat4 operator/(Mat4 lhs, const T& rhs) { return lhs /= rhs; }
        constexpr Mat4& operator/=(const T& rhs) 
        {
            for(auto& row : _data) row /= rhs;   
            return *this;
        }

        constexpr friend Mat4 operator*(const Mat4& lhs, const Mat4& rhs) 
        {
            Rows data = {};

            for(Size y = 0; y < height(); ++y)
            for(Size x = 0; x < width(); ++x)
            {
                data[y][x] = 0;

                for(Size i = 0; i < 4; ++i)
                    data[y][x] += lhs[y][i] * rhs[i][x];
            }

            return Mat4(data);
        }

        constexpr friend Column operator*(const Mat4& lhs, const Vec4<T>& rhs) 
        {
            return 
            {
                rhs.dot(lhs[0]),
                rhs.dot(lhs[1]),
                rhs.dot(lhs[2]),
                rhs.dot(lhs[3]),
            };
        }

        friend EdgeList<T> operator*(const Mat4& lhs, const EdgeList<T>& rhs) 
        {
            EdgeList<T> data = rhs;
            for(Vec4<T>& edge : data) edge = lhs * edge;
            return data;
        }

    public: // Print Support
        friend std::ostream& operator<<(std::ostream& file, const Mat4& mat)
        {
            return file << "\n" << mat[0] << "\n" << mat[1] << "\n" << mat[2] << "\n" << mat[3] << "\n";
        }
    };

    using Mat4i = Mat4<Int32>;
    using Mat4u = Mat4<UInt32>;

    using Mat4s = Mat4<Size>;

    using Mat4f = Mat4<Float32>;
    using Mat4d = Mat4<Float64>;
}
