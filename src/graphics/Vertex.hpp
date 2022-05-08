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

#include "math/Vector2D.hpp"
#include "math/Vector3D.hpp"
#include "TypeNames.hpp"
#include "Color.hpp"

namespace SPGL
{
    struct Vertex 
    {
    private:
        Vec3d _pos;
        Color _color;

        static Color VertexToColor(const Vec3d& pos)
        { return Color::HSV(pos.z * 4.0 + pos.x, 0.25); }

    public:
        Vertex() : _pos{}, _color{} {}

        Vertex(const Vec3d& pos, const Color& color)
            : _pos{pos}, _color{color} {}
        
        Vertex(const Vec3d& pos)
            : _pos{pos}, _color{VertexToColor(pos)} {}
        
    public:
        Vec2i pixel() const { return Vec2i(_pos.x + 0.5, _pos.y + 0.5); }
        Float depth() const { return _pos.z; }

        const Vec3d& pos() const { return _pos; }
        const Color& color() const { return _color; }

    public:
        constexpr Vertex& operator+=(const Vertex& rhs) { _pos += rhs._pos; _color += rhs._color; return *this; }
        constexpr Vertex& operator-=(const Vertex& rhs) { _pos -= rhs._pos; _color -= rhs._color; return *this; }
        constexpr Vertex& operator*=(const Float& rhs) { _pos *= rhs; _color *= rhs; return *this; }
        constexpr Vertex& operator/=(const Float& rhs) { _pos /= rhs; _color /= rhs; return *this; }
        
        friend Vertex operator+(Vertex lhs, const Vertex& rhs) { return lhs += rhs; }
        friend Vertex operator-(Vertex lhs, const Vertex& rhs) { return lhs -= rhs; }
        friend Vertex operator*(Vertex lhs, const Float& rhs) { return lhs *= rhs; }
        friend Vertex operator*(const Float& rhs, Vertex lhs) { return lhs *= rhs; }
        friend Vertex operator/(Vertex lhs, const Float& rhs) { return lhs /= rhs; }

    public: 
        Vertex intersect_x(const Vertex& other, int x) const
        { return Math::map(x, pixel().x, other.pixel().x, *this, other); }

        Vertex intersect_y(const Vertex& other, int y) const
        { return Math::map(y, pixel().y, other.pixel().y, *this, other); }

        Vertex intersect_z(const Vertex& other, Float z) const
        { return Math::map(z, pos().z, other.pos().z, *this, other); }
    };
}