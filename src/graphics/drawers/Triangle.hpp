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
#include <algorithm>

#include "Line.hpp"
#include "../Image.hpp"
#include "../Vector2D.hpp"

namespace SPGL
{

    class Triangle
    {
    private:
        Vec2i _t;
        Float _t_depth;
        Vec2i _m;
        Float _m_depth;
        Vec2i _b;
        Float _b_depth;

        Color _color;

    public:
        Triangle(Vec3d a, Vec3d b, Vec3d c, Color color)
        {
            if(b.y < a.y) std::swap(a, b);
            if(c.y < b.y) std::swap(b, c);
            if(b.y < a.y) std::swap(a, b);

            _t = Vec2i(std::floor(a.x), std::floor(a.y)); _t_depth = a.z;
            _m = Vec2i(std::floor(b.x), std::floor(b.y)); _m_depth = b.z;
            _b = Vec2i(std::floor(c.x), std::floor(c.y)); _b_depth = c.z;

            _color = color;
        }

    private:
        int get_side_a_x(int y) 
        {
            return _t.x + ((y - _t.y) * (_b.x - _t.x)) / (_b.y - _t.y);
        }

        int get_side_b_x(int y) 
        {
            if(y < _m.y)
                return _t.x + ((y - _t.y) * (_m.x - _t.x)) / (_m.y - _t.y);
            else
                return _b.x + ((y - _b.y) * (_m.x - _b.x)) / (_m.y - _b.y);
        }

        Float get_side_a_depth(int y) 
        {
            return _t_depth + ((y - _t.y) * (_b_depth - _t_depth)) / (_b.y - _t.y);
        }

        Float get_side_b_depth(int y) 
        {
            if(y < _m.y)
                return _t_depth + ((y - _t.y) * (_m_depth - _t_depth)) / (_m.y - _t.y);
            else
                return _b_depth + ((y - _b.y) * (_m_depth - _b_depth)) / (_m.y - _b.y);
        }

    public:
        void operator()(Image& buffer, ZBuffer& zbuffer)
        {
            for(int y = _t.y; y <= _b.y; ++y)
            {
                Line(
                    Vec2i(get_side_a_x(y), y), get_side_a_depth(y),
                    Vec2i(get_side_b_x(y), y), get_side_b_depth(y),
                    _color
                )(buffer, zbuffer);
            }
        }
    };

}