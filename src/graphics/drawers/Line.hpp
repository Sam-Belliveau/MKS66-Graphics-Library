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

#include "../FrameBuffer.hpp"
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
        Color _color_dt;

    public:
        Line(const Vertex& startv, const Vertex& endv)
        {
            Vec2i start = startv.pixel();
            Vec2i end = endv.pixel();

            Float start_depth = startv.depth();
            Float end_depth = endv.depth();

            Color start_color = startv.color();
            Color end_color = endv.color();
            
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
                std::swap(start_color, end_color);
            }

            _start = start;
            _end = end;

            _depth = start_depth;
            _depth_dt = (end_depth - start_depth) / (_end.x - _start.x);

            _color = start_color;
            _color_dt = (end_color - start_color) / (_end.x - _start.x);

            Vec2i dt = _end - _start;
            _step = dt.y < 0 ? -1 : 1;
            _dt = Vec2i(dt.x, std::abs(dt.y));
        }

    private:
        inline Color& pixel(Image& buffer, Size x, Size y) const
        {
            if(_steep) std::swap(x, y);
            return buffer(x, y); 
        }

    public:
        void operator()(FrameBuffer& scene) const
        {
            int D = (_dt.y << 1) - _dt.x;
            int y = _start.y;

            Float depth = _depth;
            Color color = _color;

            for(int x = _start.x; x <= _end.x; ++x)
            {
                scene(Vertex(Vec3d(x, y, depth), color));

                if (D > 0) 
                {
                    y += _step;
                    D -= _dt.x << 1;
                } 
                
                D += _dt.y << 1;
                depth += _depth_dt;
                color += _color_dt;
            } 
        }

        void operator()(FrameBuffer& scene, const Vec3d& normal) const
        {
            int D = (_dt.y << 1) - _dt.x;
            int y = _start.y;

            Float depth = _depth;
            Color color = _color;

            for(int x = _start.x; x <= _end.x; ++x)
            {
                scene(Vertex(Vec3d(x, y, depth), color), normal);

                if (D > 0) 
                {
                    y += _step;
                    D -= _dt.x << 1;
                } 
                
                D += _dt.y << 1;
                depth += _depth_dt;
                color += _color_dt;
            } 
        }
    };
}