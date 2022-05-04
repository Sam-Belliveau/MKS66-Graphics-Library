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
#include "Vector2D.hpp"
#include "Vector3D.hpp"
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
        Vec2i pixel() const { return Vec2i(std::round(_pos.x), std::round(_pos.y)); }
        Float depth() const { return _pos.z; }

        const Vec3d& pos() const { return _pos; }
        const Color& color() const { return _color; }

        Vertex intersect_x(const Vertex& other, int x) const
        {
            return Vertex(
                Math::map(x, pixel().x, other.pixel().x, pos(), other.pos()),
                Math::map(x, pixel().x, other.pixel().x, color(), other.color())
            );
        }

        Vertex intersect_y(const Vertex& other, int y) const
        {
            return Vertex(
                Math::map(y, pixel().y, other.pixel().y, pos(), other.pos()),
                Math::map(y, pixel().y, other.pixel().y, color(), other.color())
            );
        }

        Vertex intersect_z(const Vertex& other, Float z) const
        {
            return Vertex(
                Math::map(z, pos().z, other.pos().z, pos(), other.pos()),
                Math::map(z, pos().z, other.pos().z, color(), other.color())
            );
        }

    };
}