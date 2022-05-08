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

#include "../math/Math.hpp"
#include "../math/Vector2D.hpp"
#include "../Image.hpp"

namespace SPGL
{
    namespace FXAA
    {
        constexpr Float EDGE_THRESHOLD_MIN = 0.0312;
        constexpr Float EDGE_THRESHOLD_MAX = 0.125;
        constexpr Float SUBPIXEL_QUALITY = 0.75;

        constexpr int ITERATIONS = 12;
        

        Float luma(const Color& color)
        {
            return std::sqrt(
                color.r * 0.299 +
                color.g * 0.587 + 
                color.b * 0.114  
            );
        }

        Color get_pixel(const Image& image, const Vec2i& pos)
        {
            const Float lumaC = luma(image(pos));

            const Float lumaU = luma(image(pos + Vec2i(+0,+1)));
            const Float lumaD = luma(image(pos + Vec2i(+0,-1)));
            const Float lumaL = luma(image(pos + Vec2i(-1,+0)));
            const Float lumaR = luma(image(pos + Vec2i(+1,+0)));

            const Float luma_max = std::max({lumaC, lumaU, lumaD, lumaL, lumaR});
            const Float luma_min = std::min({lumaC, lumaU, lumaD, lumaL, lumaR});
            
            const Float luma_range = luma_max - luma_min;

            if (luma_range < std::max(EDGE_THRESHOLD_MIN, luma_max * EDGE_THRESHOLD_MAX))
            { return image(pos); }

            const Float lumaUR = luma(image(pos + Vec2i(+1,+1)));
            const Float lumaUL = luma(image(pos + Vec2i(-1,+1)));
            const Float lumaDR = luma(image(pos + Vec2i(+1,-1)));
            const Float lumaDL = luma(image(pos + Vec2i(-1,-1)));

            const Float lumaDU = lumaD + lumaU;
            const Float lumaLR = lumaL + lumaR;

            const Float lumaUC = lumaUL + lumaUR;
            const Float lumaDC = lumaDL + lumaDR;
            const Float lumaLC = lumaDL + lumaUL;
            const Float lumaRC = lumaDR + lumaUR;
            
            const Float edge_horizontal = 
                1.0 * std::abs(lumaLC - 2.0 * lumaL) + 
                2.0 * std::abs(lumaDU - 2.0 * lumaC) +
                1.0 * std::abs(lumaRC - 2.0 * lumaR);

            const Float edge_vertical = 
                1.0 * std::abs(lumaUC - 2.0 * lumaU) + 
                2.0 * std::abs(lumaLR - 2.0 * lumaC) +
                1.0 * std::abs(lumaDC - 2.0 * lumaD);

            const bool horizontal = edge_horizontal >= edge_vertical;

            const Float luma1 = horizontal ? lumaD : lumaL;
            const Float luma2 = horizontal ? lumaU : lumaR;

            const Float gradient1 = luma1 - lumaC;
            const Float gradient2 = luma2 - lumaC;

            const bool steepest = std::abs(gradient1) > std::abs(gradient2);

            const Float gradient = 0.25 * std::max(std::abs(gradient1), std::abs(gradient2));
            
            const Float step_length = steepest ? -1.0 : 1.0;
            const Float luma_avg = 0.5 * ((steepest ? luma1 : luma2) + lumaC);

            Vec2d current_pos = pos;

            if(horizontal) current_pos.y += step_length * 0.5;
            else current_pos.x += step_length * 0.5;
            
            const Vec2d offset = horizontal 
                ? Vec2d(1.0, 0.0)
                : Vec2d(0.0, 1.0);

            Vec2d pos1 = current_pos - offset;
            Vec2d pos2 = current_pos + offset;

            bool reached1 = false;
            bool reached2 = false;
            bool reached_both = false;

            Float lumaE1 = 0.0, lumaE2 = 0.0;
            
            for(int i = 0; i < ITERATIONS; ++i)
            {
                if(!reached1) lumaE1 = luma(image.interpolate(pos1)) - luma_avg;
                if(!reached2) lumaE2 = luma(image.interpolate(pos2)) - luma_avg;

                reached1 = std::abs(lumaE1) >= gradient;
                reached2 = std::abs(lumaE2) >= gradient;
                reached_both = reached1 && reached2;

                if(!reached1) pos1 -= offset;
                if(!reached2) pos2 += offset;

                if(reached_both) break;
            }
            
            const Float luma_final_avg = (1.0 / 12.0) * (2.0 * (lumaDU + lumaLR) + lumaLC + lumaRC);

            const Float subpixel_offset1 = Math::limit(std::abs(luma_final_avg - lumaC) / luma_range);
            const Float subpixel_offset2 = (-2.0 * subpixel_offset1 + 3.0) * subpixel_offset1 * subpixel_offset1;

            const Float subpixel_offset = subpixel_offset2 * subpixel_offset2 * SUBPIXEL_QUALITY;

            const Float distance1 = horizontal ? (pos.x - pos1.x) : (pos.y - pos1.y);
            const Float distance2 = horizontal ? (pos2.x - pos.x) : (pos2.y - pos.y);

            const bool direction = distance1 < distance2;

            const Float distance = std::min(distance1, distance2);
            
            const Float edge_thickness = distance1 + distance2;
            const Float pixel_offset = -distance / edge_thickness + 0.5;

            const bool correct_variation = ((direction ? lumaE1 : lumaE2) < 0.0) != (lumaC < luma_avg);

            const Float final_offset = std::max(correct_variation ? pixel_offset : 0.0, subpixel_offset);

            Vec2d final_pos = pos;

            if(horizontal) final_pos.y += final_offset * step_length;
            else final_pos.x += final_offset * step_length;

            return image.interpolate(final_pos);
        }

        Image apply(const Image& image)
        {
            Image result(image);

            for(int x = 0; x < image.width(); ++x)
            for(int y = 0; y < image.height(); ++y)
            {
                Vec2i pos(x, y);
                result(pos) = get_pixel(image, pos);
            }

            return result;
        }
    }
}