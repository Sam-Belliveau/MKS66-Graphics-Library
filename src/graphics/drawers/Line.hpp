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
    template<bool AA = false>
    class Line;

    template<>
    class Line<false>
    {
    private:
        int _step;
        bool _steep;
        Vec2i _start;
        Vec2i _end;
        Vec2i _dt;
        Color _color;

    public:
        Line(Vec2i start, Vec2i end, Color color)
        {
            _steep = std::abs(end.x - start.x) < std::abs(end.y - start.y);

            if((_steep && start.y < end.y) || (!_steep && start.x < end.x))
            { _start = start; _end = end; }
            else { _start = end; _end = start; }

            if(_steep)
            {
                std::swap(_start.x, _start.y);
                std::swap(_end.x, _end.y);
            }

            Vec2i dt = _end - _start;
            _step = dt.y < 0 ? -1 : 1;
            _dt = Vec2i(dt.x, std::abs(dt.y));

            _color = color;
        }

    private:
        inline Color& pixel(Image& buffer, Size x, Size y)
        { return _steep ? buffer(y, x) : buffer(x, y); }

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
    };

    template<>
    class Line<true>
    {
    private:
        bool _steep;
        
        Vec2d _dt;
        Float _gradient;

        Vec2d _start;
        Vec2d _start_end;
        Vec2i _start_pixel;
        Float _start_xgap;

        Vec2d _end;
        Vec2d _end_end;
        Vec2i _end_pixel;
        Float _end_xgap;

        Color _color;

    public:
        constexpr Line(Vec2d start, Vec2d end, Color color)
            : _steep{}, _dt{}, _gradient{}
            , _start{}, _start_end{}, _start_pixel{}, _start_xgap{}
            , _end{}, _end_end{}, _end_pixel{}, _end_xgap{}
            , _color{}
        {
            _steep = std::abs(end.x - start.x) < std::abs(end.y - start.y);

            if((_steep && start.y < end.y) || (!_steep && start.x < end.x))
            { _start = start; _end = end; }
            else { _start = end; _end = start; }

            if(_steep)
            {
                std::swap(_start.x, _start.y);
                std::swap(_end.x, _end.y);
            }

            _dt = _end - _start;            
            _gradient = (_dt.x == 0) ? 1.0 : (_dt.y / _dt.x);

            const Float xend1 = std::round(_start.x);
            _start_xgap = Math::rfpart(_start.x + 0.5);
            _start_end = Vec2d(xend1, _start.y + _gradient * (xend1 - _start.x));
            _start_pixel = Vec2i(std::lround(_start_end.x), Int32(_start_end.y));

            const Float xend2 = std::round(_end.x);
            _end_xgap = Math::fpart(_end.x + 0.5);
            _end_end = Vec2d(xend2, _end.y + _gradient * (xend2 - _end.x));
            _end_pixel = Vec2i(std::lround(_end_end.x), Int32(_end_end.y));

            _color = color;
        }

    private:
        inline Color& pixel(Image& buffer, Size x, Size y)
        { return _steep ? buffer(y, x) : buffer(x, y); }

    public:
        void operator()(Image& buffer)
        {            
            // End Points
            pixel(buffer, _start_pixel.x, _start_pixel.y + 0).average(_color, Math::rfpart(_start_end.y) * _start_xgap);
            pixel(buffer, _start_pixel.x, _start_pixel.y + 1).average(_color, Math::fpart(_start_end.y) * _start_xgap);
            pixel(buffer, _end_pixel.x, _end_pixel.y + 0).average(_color, Math::rfpart(_end_end.y) * _end_xgap);
            pixel(buffer, _end_pixel.x, _end_pixel.y + 1).average(_color, Math::fpart(_end_end.y) * _end_xgap);

            Float y = _start_end.y + _gradient;
            for(Int32 x = _start_pixel.x + 1; x < _end_pixel.x; ++x)
            {
                pixel(buffer, x, Int32(y) + 0).average(_color, Math::rfpart(y)); 
                pixel(buffer, x, Int32(y) + 1).average(_color, Math::fpart(y)); 
                y += _gradient;
            }
        }
    };
}