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

#include "../Image.hpp"
#include "../Vector2D.hpp"

namespace SPGL 
{

    class BresenhamLine
    {
    private:
        bool _steep;
        Vector2i _start;
        Vector2i _end;
        Color _color;
        
    public:
        BresenhamLine(Vector2i start, Vector2i end, Color color)
        {
            Vector2i dt = end - start;
            _steep = std::abs(dt.x) < std::abs(dt.y);

            if(_steep)
            {
                _start = end;
                _end = start;
            }
            else
            {
                _start = start;
                _end = end;
            }

            _color = color;
        }

    public:
        void operator()(Image& buffer)
        {
            if(_steep)
            {

            }

            else
            {
                Vector2i dt = _end - _start;
                int D = 2 * dt.y - dt.x;
                int y = _start.y;

                for(int x = _start.x; x <= _end.x; ++x)
                {
                    buffer(x, y) = _color;

                    if (D > 0) 
                    {
                        ++y;
                        D -= 2 * dt.x;
                    }

                    D += 2 * dt.y;
                } 
            }

        }
    };

}

#endif