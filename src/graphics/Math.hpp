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

#ifndef SPGL_MATH_HPP
#define SPGL_MATH_HPP

#include "TypeNames.hpp"
#include "MathConstants.hpp"

#include <cmath>

namespace SPGL
{
    namespace math
    {
        constexpr Float loop(Float x, Float range)
        {
            return x - range * std::floor(x / range);
        }

        constexpr UInt8 floatToByte(Float channel) 
        {
            if(channel <= 0.0) return 0;
            if(1.0 <= channel) return 255;
            return UInt8(std::lround(255.0 * channel));
        }

        constexpr Float byteToFloat(UInt8 channel) 
        { return channel / 255.0; }

        constexpr Float limit(Float channel, Float min = 0.0, Float max = 1.0)
        {
            if(channel <= min) return 0.0;
            if(max <= channel) return 1.0;
            else return channel;
        }
    }

}

#endif