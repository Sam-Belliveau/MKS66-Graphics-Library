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
#include "../Math.hpp"
#include "../Image.hpp"
#include "../Vector2D.hpp"

namespace SPGL
{

    class Triangle
    {
    private:
        Vertex _t;
        Vertex _m;
        Vertex _b;

        Vec3d _normal;

    public:
        Triangle(Vertex a, Vertex b, Vertex c)
        {
            Vec3d p0 = b.pos() - a.pos();
            Vec3d p1 = c.pos() - a.pos();
            _normal = p0.cross(p1).normalized();

            if(b.pos().y < a.pos().y) std::swap(a, b);
            if(c.pos().y < b.pos().y) std::swap(b, c);
            if(b.pos().y < a.pos().y) std::swap(a, b);

            _t = a;
            _m = b;
            _b = c;
        }

    public:
        const Vec3d& normal() const { return _normal; }

        void operator()(FrameBuffer& scene)
        {
            for(int y = _t.pixel().y; y < _m.pixel().y; ++y)
                Line(_t.intersect_y(_b, y), _t.intersect_y(_m, y))(scene, _normal);

            for(int y = _m.pixel().y; y <= _b.pixel().y; ++y)
                Line(_t.intersect_y(_b, y), _m.intersect_y(_b, y))(scene, _normal);
        }
    };

}