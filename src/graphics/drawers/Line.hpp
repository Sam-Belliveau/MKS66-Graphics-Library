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

#include "../Image.hpp"
#include "../Vector2D.hpp"

namespace SPGL
{

    class Line
    {
    private:
        int _step;
        bool _steep;
        Vec2i _start;
        Vec2i _end;
        Vec2i _dt;

        Float _depth;
        Float _depth_dt;

        Color _color;

    public:
        Line(Vec2i start, Float start_depth, Vec2i end, Float end_depth, Color color)
        {
            _steep = std::abs(end.x - start.x) < std::abs(end.y - start.y);

            if(_steep)
            {
                std::swap(start.x, start.y);
                std::swap(end.x, end.y);
            }

            if(end.x < start.x) 
            {
                std::swap(start, end);
                std::swap(start_depth, end_depth);
            }

            _start = start;
            _end = end;

            _depth = start_depth;
            _depth_dt = (end_depth - start_depth) / (_end.x - _start.x);

            Vec2i dt = _end - _start;
            _step = dt.y < 0 ? -1 : 1;
            _dt = Vec2i(dt.x, std::abs(dt.y));

            _color = color;
        }

        Line(Vec2i start, Vec2i end, Color color)
            : Line{start, 0.0, end, 0.0, color} {} 

        Line(Vec3d start, Vec3d end, Color color)
            : Line{
                Vec2i(std::floor(start.x), std::floor(start.y)), start.z, 
                Vec2i(std::floor(end.x), std::floor(end.y)), end.z, 
                color
            } {} 

    private:
        inline Color& pixel(Image& buffer, Size x, Size y)
        {
            if(_steep) std::swap(x, y);
            return buffer(x, y); 
        }


        inline Color& pixel(Image& buffer, ZBuffer& zbuffer, Size x, Size y, Float z)
        {
            if(_steep) std::swap(x, y);

            if(zbuffer.set(x, y, z)) 
                return buffer(x, y);
            else
                return buffer(-1, -1);
        }

    public:
        void operator()(Image& buffer)
        {
            int D = (_dt.y << 1) - _dt.x;
            int y = _start.y;

            for(int x = _start.x; x <= _end.x; ++x)
            {
                pixel(buffer, x, y) = _color;

                if (D > 0) 
                {
                    y += _step;
                    D -= _dt.x << 1;
                } 
                
                D += _dt.y << 1;
            } 
        }

        void operator()(Image& buffer, ZBuffer& zbuffer)
        {
            int D = (_dt.y << 1) - _dt.x;
            int y = _start.y;

            Float depth = _depth;

            for(int x = _start.x; x <= _end.x; ++x)
            {
                pixel(buffer, zbuffer, x, y, depth) = _color;

                if (D > 0) 
                {
                    y += _step;
                    D -= _dt.x << 1;
                } 
                
                D += _dt.y << 1;
                depth += _depth_dt;
            } 
        }
    };
}