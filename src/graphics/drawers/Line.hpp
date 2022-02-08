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

#ifndef SPGL_LINE_HPP
#define SPGL_LINE_HPP 1

#include <cmath>
#include <algorithm>

#include "../Image.hpp"
#include "../Vector2D.hpp"

namespace SPGL 
{
    template<bool Fast = false>
    class Line
    {
    private:
        int _step;
        bool _steep;
        Vector2i _start;
        Vector2i _end;
        Vector2i _dt;
        Color _color;

    public:
        Line(Vector2i start, Vector2i end, Color color)
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

            Vector2i dt = _end - _start;
            _step = dt.y < 0 ? -1 : 1;
            _dt = Vector2i(dt.x, std::abs(dt.y));

            _color = color;
        }

    private:
        void fast(Image& buffer)
        {
            int D = (_dt.y << 1) - _dt.x;
            int y = _start.y;

            for(int x = _start.x; x <= _end.x; ++x)
            {
                if(_steep) buffer(y, x) = _color;
                else buffer(x, y) = _color;

                if (D > 0) 
                {
                    y += _step;
                    D -= _dt.x << 1;
                } 
                
                D += _dt.y << 1;
            } 
        }

        void slow(Image& buffer)
        {
            // TODO: Wu's Algorithm
            fast(buffer);
        }

    public:
        void operator()(Image& buffer)
        {
            if(Fast) fast(buffer);
            else slow(buffer);
        }
    };

    class SlowLine
    {
    private:
        int _step;
        bool _steep;
        Vector2i _start;
        Vector2i _end;
        Vector2i _dt;
        Color _color;
        
    public:
        SlowLine(Vector2i start, Vector2i end, Color color)
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

            Vector2i dt = _end - _start;
            _step = dt.y < 0 ? -1 : 1;
            _dt = Vector2i(dt.x, std::abs(dt.y));

            _color = color;
        }

    public:
        void operator()(Image& buffer)
        {
            int D = (_dt.y << 1) - _dt.x;
            int y = _start.y;

            for(int x = _start.x; x <= _end.x; ++x)
            {
                if(_steep) buffer(y, x) = _color;
                else buffer(x, y) = _color;

                if (D > 0) 
                {
                    y += _step;
                    D -= _dt.x << 1;
                } 
                
                D += _dt.y << 1;
            } 
        }
    };

}

#endif