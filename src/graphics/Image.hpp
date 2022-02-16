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

#include <vector> // std::vector
#include <algorithm> // std::copy
#include <stdexcept> // std::out_of_range
#include <iterator> // std::reverse_iterator

#include "TypeNames.hpp"
#include "Vector2D.hpp"
#include "Color.hpp"

namespace SPGL // Definitions
{
    class Image
    {
    public: /* Container Information */
        using value_type = Color;

        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        using reference = value_type&;
        using const_reference = const value_type&;

        using pointer = value_type*;
        using const_pointer = const value_type*;

        using iterator = std::vector<value_type>::iterator;
        using const_iterator = std::vector<value_type>::const_iterator;

        using reverse_iterator = std::vector<value_type>::reverse_iterator;
        using const_reverse_iterator = std::vector<value_type>::const_reverse_iterator;

    public: /* Information */
        Vec2s vecsize() const { return _img_size; }
        Size height() const { return _img_size.y; }
        Size width()  const { return _img_size.x; }
        bool empty()  const { return (_img_size.x | _img_size.y) == 0; }
        Size size()   const { return _img_size.x * _img_size.y; }

        const value_type* data() const { return _img_data.data(); }
        value_type* data() { return _img_data.data(); }

    public: /* Constructors */
        // Default Constructor
        Image() {}

        // Copy/Move Constructors
        Image(const Image& in) = default;
        Image(Image&& in) = default;
        Image& operator=(const Image& in) = default;
        Image& operator=(Image&& in) = default;

        // Create Functions
        Image(Size x, Size y, Color color = Color(0,0,0))
            : _img_size{x, y}
            , _img_data{std::vector<Color>(x * y, color)}
            , _garbage{} {}

    public: /* Accessors */
        /*** Single value indexing ***/
        /***/ value_type& operator[](Size i) /***/ { return _img_data[i]; }
        const value_type& operator[](Size i) const { return _img_data[i]; }
        /***/ value_type& operator()(Size i) /***/ { return _img_data[i]; }
        const value_type& operator()(Size i) const { return _img_data[i]; }

        /*** Double value indexing ***/
        /***/ value_type& get(Size x, Size y)
        {
            if(width()  <= x) { return _garbage; }
            if(height() <= y) { return _garbage; }
            return _img_data[y * width() + x];
        }

        const value_type& get(Size x, Size y) const 
        {
            if(width()  <= x) { return _garbage; }
            if(height() <= y) { return _garbage; }
            return _img_data[y * width() + x];
        }
        
        /***/ value_type& get(Vec2s i)     /***/ { return get(i.x, i.y); }
        const value_type& get(Vec2s i)     const { return get(i.x, i.y); }
        
        /***/ value_type& operator()(Size x, Size y) /***/ { return get(x, y); }
        const value_type& operator()(Size x, Size y) const { return get(x, y); }

        /***/ value_type& operator[](Vec2s i) /***/ { return get(i.x, i.y); }
        const value_type& operator[](Vec2s i) const { return get(i.x, i.y); }
        /***/ value_type& operator()(Vec2s i) /***/ { return get(i.x, i.y); }
        const value_type& operator()(Vec2s i) const { return get(i.x, i.y); }

    private: /* Raw Data */
        std::vector<value_type> _img_data;
        Vec2s _img_size;
        Color _garbage;

    public: /* Iterators */
        auto begin() { return std::begin(_img_data); }
        auto begin() const { return std::cbegin(_img_data); }
        auto cbegin() const { return std::cbegin(_img_data); }

        auto end() { return std::end(_img_data); }
        auto end() const { return std::cend(_img_data); }
        auto cend() const { return std::cend(_img_data); }

        auto rbegin() { return std::reverse_iterator(std::end(_img_data)); }
        auto rbegin() const { return std::reverse_iterator(std::cend(_img_data)); }
        auto crbegin() const { return std::reverse_iterator(std::cend(_img_data)); }

        auto rend() { return std::reverse_iterator(std::begin(_img_data)); }
        auto rend() const { return std::reverse_iterator(std::cbegin(_img_data)); }
        auto crend() const { return std::reverse_iterator(std::cbegin(_img_data)); }

    public: /* Modifications  */
        Image dither(const std::function<Color(Color)>) const;
        Image resize_nearest(const Size x, const Size y) const;
    };
}

namespace SPGL
{
    Image Image::dither(const std::function<Color(Color)> rounder) const 
    {
        Image result = Image(*this);

        constexpr Color::RepT RATIO_7_48  = 7.0 / 48.0;
        constexpr Color::RepT RATIO_5_48  = 5.0 / 48.0;
        constexpr Color::RepT RATIO_3_48  = 3.0 / 48.0;
        constexpr Color::RepT RATIO_1_48  = 1.0 / 48.0;

        for(long x = 0; x < width();  ++x)
        for(long y = 0; y < height(); ++y)
        {
            const Color pixel = result(x, y);
            const Color round = rounder(pixel);

            result(x + 0, y + 0) = round;

            const Color error = (pixel - round);
            result(x + 0, y + 1) += error * RATIO_7_48;
            result(x + 0, y + 2) += error * RATIO_5_48;
            
            result(x + 1, y - 2) += error * RATIO_3_48;
            result(x + 1, y - 1) += error * RATIO_5_48;
            result(x + 1, y + 0) += error * RATIO_7_48;
            result(x + 1, y + 1) += error * RATIO_5_48;
            result(x + 1, y + 2) += error * RATIO_3_48;
            
            result(x + 2, y - 2) += error * RATIO_1_48;
            result(x + 2, y - 1) += error * RATIO_3_48;
            result(x + 2, y + 0) += error * RATIO_5_48;
            result(x + 2, y + 1) += error * RATIO_3_48;
            result(x + 2, y + 2) += error * RATIO_1_48;
        }

        return std::move(result);
    }

    Image Image::resize_nearest(const Size x, const Size y) const
    {
        Image result(x, y);

        for(Size ix = 0; ix < x; ++ix)
        for(Size iy = 0; iy < y; ++iy)
        {
            const Size sx = (ix * width()) / x;
            const Size sy = (iy * height()) / y;

            result(ix, iy) = get(sx, sy);
        }

        return result;
    }
}