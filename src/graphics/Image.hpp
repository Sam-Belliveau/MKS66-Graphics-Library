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

#include <iostream> // std::ostream
#include <vector> // std::vector
#include <algorithm> // std::copy
#include <stdexcept> // std::out_of_range
#include <iterator> // std::reverse_iterator

#include "math/Vector2D.hpp"
#include "TypeNames.hpp"
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
            return _img_data[(height() - 1 - y) * width() + x];
        }

        const value_type& get(Size x, Size y) const 
        {
            if(width()  <= x) { return _garbage; }
            if(height() <= y) { return _garbage; }
            return _img_data[(height() - 1 - y) * width() + x];
        }
        
        /***/ value_type& get(Vec2s i)     /***/ { return get(i.x, i.y); }
        const value_type& get(Vec2s i)     const { return get(i.x, i.y); }
        
        /***/ value_type& operator()(Size x, Size y) /***/ { return get(x, y); }
        const value_type& operator()(Size x, Size y) const { return get(x, y); }

        /***/ value_type& operator[](Vec2s i) /***/ { return get(i.x, i.y); }
        const value_type& operator[](Vec2s i) const { return get(i.x, i.y); }
        /***/ value_type& operator()(Vec2s i) /***/ { return get(i.x, i.y); }
        const value_type& operator()(Vec2s i) const { return get(i.x, i.y); }

        value_type interpolate(Vec2d i) const
        {
            Float x_f = Math::fpart(i.x);
            Float y_f = Math::fpart(i.y);
            
            Float x_rf = 1.0 - x_f;
            Float y_rf = 1.0 - y_f;

            Vec2i pos(std::floor(i.x), std::floor(i.y));

            return (
                x_rf * y_rf * get(pos + Vec2i(0, 0)) +
                x_f  * y_rf * get(pos + Vec2i(1, 0)) +
                x_rf * y_f  * get(pos + Vec2i(0, 1)) +
                x_f  * y_f  * get(pos + Vec2i(1, 1)) 
            );
        } 

        value_type sample_box(Vec2d beg, Vec2d end) const
        {
            if(end.x < beg.x) std::swap(beg.x, end.x);
            if(end.y < beg.y) std::swap(beg.y, end.y);
         
            const Size fy_beg = std::floor(beg.y);
            const Size fy_end = std::floor(end.y);
            const Size fx_beg = std::floor(beg.x);
            const Size fx_end = std::floor(end.x);

            const Float px_beg = 1 + fx_beg - beg.x;
            const Float py_beg = 1 + fy_beg - beg.y;
            
            const Float px_end = end.x - fx_end;
            const Float py_end = end.y - fy_end;

            ColorAverage color;   
            
            color.add(get(fx_beg, fy_beg), px_beg * py_beg);
            color.add(get(fx_end, fy_beg), px_end * py_beg);
            color.add(get(fx_beg, fy_end), px_beg * py_end);
            color.add(get(fx_end, fy_end), px_end * py_end);

            for(Size x = fx_beg + 1; x < fx_end; ++x) 
            {
                color.add(get(x, fy_beg), py_beg);
                color.add(get(x, fy_end), py_end);
            }

            for(Size y = fy_beg + 1; y < fy_end; ++y) 
            {
                color.add(get(fx_beg, y), px_beg);
                color.add(get(fx_end, y), px_end);
                for(Size x = fx_beg + 1; x < fx_end; ++x) 
                    color.add(get(x, y));
            }

            return color.result();
        }

        value_type sample_box_luma(Vec2d beg, Vec2d end) const
        {
            if(end.x < beg.x) std::swap(beg.x, end.x);
            if(end.y < beg.y) std::swap(beg.y, end.y);
         
            const Size fy_beg = std::floor(beg.y);
            const Size fy_end = std::floor(end.y);
            const Size fx_beg = std::floor(beg.x);
            const Size fx_end = std::floor(end.x);

            const Float px_beg = 1 + fx_beg - beg.x;
            const Float py_beg = 1 + fy_beg - beg.y;
            
            const Float px_end = end.x - fx_end;
            const Float py_end = end.y - fy_end;

            ColorAverage color;   
            
            color.add(get(fx_beg, fy_beg), px_beg * py_beg);
            color.add(get(fx_end, fy_beg), px_end * py_beg);
            color.add(get(fx_beg, fy_end), px_beg * py_end);
            color.add(get(fx_end, fy_end), px_end * py_end);

            for(Size x = fx_beg + 1; x < fx_end; ++x) 
            {
                color.add(get(x, fy_beg), py_beg);
                color.add(get(x, fy_end), py_end);
            }

            for(Size y = fy_beg + 1; y < fy_end; ++y) 
            {
                color.add(get(fx_beg, y), px_beg);
                color.add(get(fx_end, y), px_end);
                for(Size x = fx_beg + 1; x < fx_end; ++x) 
                    color.add(get(x, y));
            }

            return color.result();
        }

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
        Image dither(const std::function<Color(Color)>, const Color::RepT = 1.0) const;
        Image dither_fast(const std::function<Color(Color)>) const;
        Image resize_nearest(const Size x, const Size y) const;
        Image resize_linear(const Size x, const Size y) const;
        Image resize_samples(const Size x, const Size y) const;
        Image gaussian_blur(const int radius) const;
        Image box_blur(const int radius) const;
    };
}

namespace SPGL
{
    template<Size bits>
    static UInt8 round_byte(UInt8& byte)
    {
        byte &= ~(0xff >> bits);
        byte |= byte >> (1 * bits);
        byte |= byte >> (2 * bits);
        byte |= byte >> (2 * bits);
        byte |= byte >> (2 * bits);
        return byte;
    }

    template<Size r_bits, Size g_bits, Size b_bits>
    Color rgb_bit_pallet(const Color& color) 
    {
        Color::Bytes bytes = color;
        round_byte<r_bits>(bytes.r);
        round_byte<g_bits>(bytes.g);
        round_byte<b_bits>(bytes.b);
        return bytes;
    }
    
    template<Size bits>
    Color grayscale_bit_color(const Color& color) 
    {
        Color::Bytes bytes = color.grayscale();
        round_byte<bits>(bytes.r);
        round_byte<bits>(bytes.g);
        round_byte<bits>(bytes.b);
        return bytes;
    }

    Image Image::dither(const std::function<Color(Color)> rounder, const Color::RepT error_mul) const 
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

            const Color error = (pixel - round) * error_mul;
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

    Image Image::dither_fast(const std::function<Color(Color)> rounder) const 
    {
        Image result = Image(*this);

        constexpr Color::RepT RATIO_1_8  = 1.0 / 8.0;

        for(long x = 0; x < width();  ++x)
        for(long y = 0; y < height(); ++y)
        {
            const Color pixel = result(x, y);
            const Color round = rounder(pixel);

            result(x + 0, y + 0) = round;

            const Color error = (pixel - round) * RATIO_1_8;
            result(x + 0, y + 1) += error;
            result(x + 0, y + 2) += error;
            result(x + 1, y - 1) += error;
            result(x + 1, y + 0) += error;
            result(x + 1, y + 1) += error;
            result(x + 2, y + 0) += error;
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

    Image Image::resize_linear(const Size x, const Size y) const
    {
        Image result(x, y);

        for(Size ix = 0; ix < x; ++ix)
        for(Size iy = 0; iy < y; ++iy)
        {
            result(ix, iy) = interpolate(Vec2d(
                Float(ix * width()) / x, 
                Float(iy * height()) / y
            ));
        }

        return result;
    }

    Image Image::resize_samples(const Size x, const Size y) const
    {
        Image result(x, y);

        for(Size ix = 0; ix < x; ++ix)
        for(Size iy = 0; iy < y; ++iy)
        {
            result(ix, iy) = sample_box(
                Vec2d(
                    Float((ix + 0.0) * width()) / x, 
                    Float((iy + 0.0) * height()) / y
                ),

                Vec2d(
                    Float((ix + 1.0) * width()) / x, 
                    Float((iy + 1.0) * height()) / y
                )
            );
        }

        return result;
    }
}

namespace SPGL
{
    Image Image::box_blur(const int radius) const
    { 
        Image out_h(width(), height());
        Image out(width(), height());

        for(int x = 0; x < width(); ++x)
        {
            ColorAverage pixel;
            for(int iy = 0 - radius; iy < 0 + radius; ++iy)
                pixel.add(get(x, iy));

            for(int y = 0; y < height(); ++y)
            {
                pixel.add(get(x, y + radius));
                out_h(x, y) = pixel.result();
                pixel.sub(get(x, y - radius));
            }
        }

        for(int y = 0; y < height(); ++y)
        {
            ColorAverage pixel;
            for(int ix = 0 - radius; ix < 0 + radius; ++ix)
                pixel.add(out_h(ix, y));

            for(int x = 0; x < width(); ++x)
            {
                pixel.add(out_h(x + radius, y));
                out(x, y) = pixel.result();
                pixel.sub(out_h(x - radius, y));
            }
        }

        return out;
    }

    Image Image::gaussian_blur(const int radius) const
    {
        Image result(*this);
        const int w = std::sqrt(radius);

        for(int i = 0; i < w; ++i)
            result = result.box_blur(w);

        return result;
    }
}

namespace SPGL
{
    namespace PPM
    {
        const char* MAGIC_HEADER = "P6\n";
        const Size COLOR_DEPTH = 255;
    }
}

namespace SPGL
{
    std::ostream& operator<<(std::ostream& file, const Image& image)
    {
        file << PPM::MAGIC_HEADER;
        file << image.width() << ' ' << image.height() << '\n';
        file << PPM::COLOR_DEPTH << '\n';
        
        for(const Color& color : image)
        { 
            Color::Bytes raw = color.bytes();
            file.write(reinterpret_cast<const char*>(&raw.r), 1);
            file.write(reinterpret_cast<const char*>(&raw.g), 1);
            file.write(reinterpret_cast<const char*>(&raw.b), 1);
        }
    
        file << '\n';

        return file;
    }
    
    std::istream& operator>>(std::istream& file, Image& image)
    {
        std::string type;
        Size sx, sy;
        std::string depth;

        file >> type;
        file >> sx >> sy;
        file >> depth;

        UInt8 nl;
        file.read(reinterpret_cast<char*>(&nl), 1);
        
        image = Image(sx, sy);

        for(Size i = 0; i < sx * sy; ++i)
        {
            UInt8 r, g, b;
            file.read(reinterpret_cast<char*>(&r), 1);
            file.read(reinterpret_cast<char*>(&g), 1);
            file.read(reinterpret_cast<char*>(&b), 1);
            image[i] = Color::Bytes(r, g, b);
        }

        return file;
    }
}