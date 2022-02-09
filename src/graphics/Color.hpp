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

#ifndef SPGL_COLOR_HPP
#define SPGL_COLOR_HPP 1

#include <algorithm> // std::max

#include "TypeNames.hpp"
#include "Math.hpp"

namespace SPGL // Definitions
{
    struct Color
    {
    public: /* HSV Class */
        struct HSV
        {
        public: // Methods
            // Default Constructor
            constexpr HSV() : v{0.0}, s{0.0}, h{0.0} {}

            // Copy Constructors
            HSV(const HSV &in) = default;
            HSV& operator=(const HSV &in) = default;

            // Custom Constructors
            constexpr HSV(const Float ih, const Float is = 1.0, const Float iv = 1.0)
                : h{math::loop(ih, 360.0)}, s{math::limit(is)}, v{math::limit(iv)} {}

            constexpr HSV(const Color in) : h{0.0}, s{0.0}, v{0.0}
            {
                const Float min = std::min({in.r, in.g, in.b});
                const Float max = std::max({in.r, in.g, in.b});
                const Float delta = max - min;

                v = max;
                if(1e-4 < delta && 0.0 < max)
                {
                    s = delta / max;
                    /**/ if(max <= in.r) h = 0.0 + (in.g - in.b) / delta;
                    else if(max <= in.g) h = 2.0 + (in.b - in.r) / delta;
                    else if(max <= in.b) h = 4.0 + (in.r - in.g) / delta;
                    h = 60.0 * math::loop(h, 6.0);                    
                }
            }

        public: // Variables
            Float h, s, v;
        };  
    
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

            constexpr Bytes(const Color in)
                : r{math::floatToByte(in.r)}
                , g{math::floatToByte(in.g)}
                , b{math::floatToByte(in.b)} {}

        public: // Variables
            UInt8 r, g, b;
        };

    private: /* Helper Functions */
        constexpr Color& clamp()
        {
            if(r < 0.0) r = 0.0;
            if(g < 0.0) g = 0.0;
            if(b < 0.0) b = 0.0;
            return *this;
        }

    public: /* Functions */
        // Default Constructor
        constexpr Color() : r{0.0}, g{0.0}, b{0.0} {}

        // Copy Constructor
        Color(const Color &in) = default;
        Color& operator=(const Color &in) = default;

        // RGB Constructor
        constexpr Color(const Float ir, const Float ig, const Float ib)
                        : r{math::limit(ir)}
                        , g{math::limit(ig)}
                        , b{math::limit(ib)} { }

        // HSV Constructor
        constexpr Color(const HSV in) : r{in.v}, g{in.v}, b{in.v}
        {
            if (0.0 < in.s)
            {
                const Float hue = math::loop(in.h / 60.0, 6.0);
                const Float fract = math::loop(hue, 1.0);
                
                const Float p = in.v * (1.0 - (in.s));
                const Float q = in.v * (1.0 - (in.s * fract));
                const Float t = in.v * (1.0 - (in.s * (1.0 - fract)));

                switch(long(hue))
                {
                    case 0: r = in.v; g = t; b = p; break;
                    case 1: r = q; g = in.v; b = p; break;
                    case 2: r = p; g = in.v; b = t; break;
                    case 3: r = p; g = q; b = in.v; break;
                    case 4: r = t; g = p; b = in.v; break;
                    default: r = in.v; g = p; b = q; break;
                }
            }
        }

        // Bytes Constructor
        constexpr Color(const Bytes in) 
            : r{math::byteToFloat(in.r)}
            , g{math::byteToFloat(in.g)}
            , b{math::byteToFloat(in.b)} {}

        // Grayscale Constructor
        constexpr Color(const Float in) 
            : r{math::limit(in)}
            , g{math::limit(in)}
            , b{math::limit(in)} {}

    public: /* Variables */
        Float r, g, b; 

    public: /* Math Operators */
        constexpr Color& operator+=(const Color& rhs)
        {
            r += rhs.r; g += rhs.g; b += rhs.b;
            return clamp();
        }

        constexpr Color& operator-=(const Color& rhs)
        {
            r -= rhs.r; g -= rhs.g; b -= rhs.b;
            return clamp();
        }

        template<typename T>
        constexpr Color& operator*=(const T& rhs)
        {
            r *= rhs; g *= rhs; b *= rhs;
            return clamp();
        }

        template<typename T>
        constexpr Color& operator/=(const T& rhs)
        {
            r /= rhs; g /= rhs; b /= rhs;
            return clamp();
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

    template<typename T>
    constexpr Color operator*(Color lhs, const T& rhs) 
    { return lhs *= rhs; }

    template<typename T>
    constexpr Color operator*(const T& lhs, Color rhs) 
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

#endif // SPGL_COLOR_HPP