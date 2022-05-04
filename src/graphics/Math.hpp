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
#include "MathConstants.hpp"

#include <cmath>

namespace SPGL
{
    namespace Math
    {
        template<class FT>
        inline constexpr FT loop(FT x, const FT range = FT(1.0))
        {
            const FT inv = FT(1.0) / range; 
            return x - range * std::floor(x * inv); 
        }

        template<class FT>
        inline constexpr UInt8 float_to_byte(FT channel) 
        {
            if(channel <= FT(0.0)) return 0;
            if(FT(1.0) <= channel) return 255;
            return UInt8(std::lround(FT(255.0) * channel));
        }

        template<class FT>
        inline constexpr FT byte_to_float(UInt8 channel) 
        { return channel / FT(255.0); }

        template<class FT>
        inline constexpr FT limit(FT channel, FT min = FT(0.0), Float max = FT(1.0))
        {
            if(channel <= min) return min;
            if(max <= channel) return max;
            else return channel;
        }

        template<class FT>
        inline constexpr FT fpart(FT x)
        { return x - std::floor(x); }

        template<class FT>
        inline constexpr FT rfpart(FT x)
        { return FT(1.0) - fpart(x); }

        template<class A, class B, class C>
        inline constexpr C map(A x, B input_start, B input_end, C output_start, C output_end)
        { 
            return (input_start == input_end) 
                ? output_start 
                : output_start + ((x - input_start) * (output_end - output_start)) / (input_end - input_start); 
        }

        template<class A, class B, class C>
        inline constexpr C map_x_to_y(A x, const Vec2<B>& start, const Vec2<C>& end)
        { return map(x, start.x, end.x, start.y, end.y); }

        template<class A, class B, class C>
        inline constexpr C map_y_to_x(A y, const Vec2<B>& start, const Vec2<C>& end)
        { return map(y, start.y, end.y, start.x, end.x); }
    }

}