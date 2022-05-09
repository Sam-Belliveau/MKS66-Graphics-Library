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

#include <initializer_list>
#include <algorithm>
#include <limits>
#include <random>

#include "math/Math.hpp"
#include "TypeNames.hpp"

namespace SPGL // Definitions
{
    struct Color
    {
    public: /* Type Definition */
        using RepT = Float64;

    public: /* Bytes Class */
        struct Bytes
        {
        public: // Methods
            // Default Constructor
            constexpr Bytes() : r{0}, g{0}, b{0} {}

            // Copy Constructors
            Bytes(const Bytes &in) = default;
            Bytes& operator=(const Bytes &in) = default;

            // Custom Constructors
            constexpr Bytes(const UInt8 ir, const UInt8 ig, const UInt8 ib)
                : r{ir}, g{ig}, b{ib} {}

            constexpr Bytes(const Color& in)
                : r{Math::float_to_byte(in.r)}
                , g{Math::float_to_byte(in.g)}
                , b{Math::float_to_byte(in.b)} {}

        public: // Variables
            UInt8 r, g, b;
        };

    private: /* Helper Functions */
        // Fix color class in any way needed
        constexpr Color& clamp() { return *this; }

    public: /* Functions */
        // Default Constructor
        constexpr Color() : r{0.0}, g{0.0}, b{0.0} {}

        // Copy Constructor
        constexpr Color(const Color &in) = default;
        constexpr Color& operator=(const Color &in) = default;

        // RGB Constructor
        constexpr Color(const RepT ir, const RepT ig, const RepT ib)
                        : r{Math::limit(ir)}
                        , g{Math::limit(ig)}
                        , b{Math::limit(ib)} { }
        
        // Grayscale Constructor
        constexpr Color(const RepT lum)
                        : r{Math::limit(lum)}
                        , g{Math::limit(lum)}
                        , b{Math::limit(lum)} { }

        // HSV Constructor
        constexpr static Color HSV(const RepT h, RepT s = RepT(1.0), RepT v = RepT(1.0))
        {
            if (s <= RepT(0.0)) s = RepT(0.0);
            if (s >= RepT(1.0)) s = RepT(1.0);

            if (v <= RepT(0.0)) v = RepT(0.0);
            if (v >= RepT(1.0)) v = RepT(1.0);

            if (RepT(0.0) <= s)
            {
                constexpr RepT SCALE = RepT(1.0) / RepT(60.0);
                const RepT hue = Math::loop(h * SCALE, RepT(6.0));
                const RepT fract = Math::loop(hue);
                
                const RepT p = v * (RepT(1.0) - (s));
                const RepT q = v * (RepT(1.0) - (s * fract));
                const RepT t = v * (RepT(1.0) - (s * (RepT(1.0) - fract)));

                switch(long(hue))
                {
                    case 0:  return Color(v, t, p);
                    case 1:  return Color(q, v, p);
                    case 2:  return Color(p, v, t);
                    case 3:  return Color(p, q, v);
                    case 4:  return Color(t, p, v);
                    default: return Color(v, p, q);
                }
            }

            return Color(v);
        }

        static Color RandomHue() 
        {
            static std::default_random_engine rng;
            static std::uniform_real_distribution hue(0, 360);
            return HSV(hue(rng), 1.0, 1.0);
        }

        static Color RandomHSV() 
        {
            static std::default_random_engine rng;
            static std::uniform_real_distribution hue(0.0, 360.0);
            static std::uniform_real_distribution scalar(0.0, 1.0);
            return HSV(hue(rng), scalar(rng), scalar(rng));
        }

        static Color Random() 
        {
            static std::default_random_engine rng;
            static std::uniform_real_distribution scalar(0.0, 1.0);
            return Color(scalar(rng), scalar(rng), scalar(rng));
        }

        // Bytes Constructor
        constexpr Color(const Bytes in) 
            : r{Math::byte_to_float<RepT>(in.r)}
            , g{Math::byte_to_float<RepT>(in.g)}
            , b{Math::byte_to_float<RepT>(in.b)} {}

    public: /* Variables */
        RepT r, g, b; 

    public: /* Conversion to Byte Type */
        constexpr Bytes bytes() const
        { return Bytes(*this); }

    public: /* Math Operators */
        constexpr static Color average(Color lhs, const Color& rhs, const RepT weight = 0.5)
        { return lhs.average(rhs, weight); }
        
        constexpr Color& average(Color rhs, const RepT weight = 0.5)
        { return operator*=(RepT(1.0) - weight).operator+=(rhs *= weight); }

        constexpr Color& operator+=(const Color& rhs)
        { r += rhs.r; g += rhs.g; b += rhs.b; return clamp(); }

        constexpr Color& operator-=(const Color& rhs)
        { r -= rhs.r; g -= rhs.g; b -= rhs.b; return clamp(); }

        constexpr Color& operator*=(const Color& rhs)
        { r *= rhs.r; g *= rhs.g; b *= rhs.b; return clamp(); }

        constexpr Color& operator*=(const RepT rhs)
        { r *= rhs; g *= rhs; b *= rhs; return clamp(); }

        constexpr Color& operator/=(const RepT rhs)
        { r /= rhs; g /= rhs; b /= rhs; return clamp(); }

        constexpr Color clamped() const
        { return Color(Math::limit(r), Math::limit(g), Math::limit(b)); }

    public: /* Match Function */
        constexpr RepT luma() const
        {
            return std::pow(
                r * 0.299 + g * 0.587 + b * 0.114,
                (1.0 / 2.22)
            );
        }

        constexpr RepT distance(const Color other) const
        {
            const RepT dr = r - other.r;
            const RepT dg = g - other.g;
            const RepT db = b - other.b;
            return dr * dr + dg * dg + db * db;
        }

        template<class Container>
        Color match(const Container& colors) const
        {
            Color result = *std::begin(colors);
            Color::RepT min_error = std::numeric_limits<RepT>::max();

            for(const Color& color : colors)
            {
                const Color::RepT error = distance(color);
                if(error < min_error)
                {
                    min_error = error;
                    result = color;
                }
            }

            return result;
        }

        Color match(const std::initializer_list<Color> colors)
        { return match<std::initializer_list<Color>>(colors); }

        Color grayscale() const
        { 
            RepT lum = 
                0.2126 * r +
                0.7152 * g +
                0.0722 * b;
            return Color(lum);
        }

    public: /* Static Colors */
        static const Color Black, White;
        static const Color Red, Green, Blue;
        static const Color Orange, Yellow, Cyan, Purple;
    };
}

namespace SPGL // Implementation
{
    // Operators
    constexpr bool operator==(const Color a, const Color b)
    { return (a.r == b.r) && (a.g == b.g) && (a.b == b.b); }

    constexpr bool operator!=(const Color a, const Color b)
    { return !(a == b); }

    // Math Operators
    constexpr Color operator+(Color lhs, const Color& rhs) 
    { return lhs += rhs; }

    constexpr Color operator-(Color lhs, const Color& rhs) 
    { return lhs -= rhs; }

    constexpr Color operator*(Color lhs, const Color& rhs) 
    { return lhs *= rhs; }

    constexpr Color operator*(Color lhs, const Color::RepT rhs) 
    { return lhs *= rhs; }

    constexpr Color operator*(const Color::RepT lhs, Color rhs) 
    { return rhs *= lhs; }

    template<typename T>
    constexpr Color operator/(Color lhs, const T& rhs) 
    { return lhs /= rhs; }

    // Static Variables
    const Color Color::Black = Color(0.0);
    const Color Color::White = Color(1.0);

    const Color Color::Red   = Color(1.0, 0.0, 0.0);
    const Color Color::Green = Color(0.0, 1.0, 0.0);
    const Color Color::Blue  = Color(0.0, 0.0, 1.0);

    const Color Color::Orange = Color(1.0, 0.5, 0.0);
    const Color Color::Yellow = Color(1.0, 1.0, 0.0);
    const Color Color::Cyan   = Color(0.0, 1.0, 1.0);
    const Color Color::Purple = Color(1.0, 0.0, 1.0);
}