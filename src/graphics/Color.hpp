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

#include "./TypeNames.hpp"

namespace SPGL // Definitions
{
    struct Color
    {
    private: /* Helper Functions */
        constexpr static UInt8 getByte(Float channel) 
        {
            if(channel <= 0.0) return 0;
            if(1.0 <= channel) return 255;
            return UInt8(255.0 * channel);
        }

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
                : h{ih}, s{is}, v{iv} {}

            constexpr HSV(const Color in)
                : v{0}, s{0}, h{0}
            {
                /** TODO **/
            }

        public: // Variables
            Float h, s, v;
        };  
    
        struct Byte 
        {
        public: // Methods
            // Default Constructor
            constexpr Byte() : r{0}, g{}, b{0} {}

            // Copy Constructors
            Byte(const Byte &in) = default;
            Byte& operator=(const Byte &in) = default;

            // Custom Constructors
            constexpr Byte(const UInt8 ir, const UInt8 ig, const UInt8 ib)
                : r{ir}, g{ig}, b{ib} {}

            constexpr Byte(const Color in)
                : r{getByte(in.r)}, g{getByte(in.g)}, b{getByte(in.b)} {}

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
        constexpr Color() : b{0.0}, g{0.0}, r{0.0} {}

        // Copy Constructor
        Color(const Color &in) = default;
        Color& operator=(const Color &in) = default;

        // RGBA Constructor
        constexpr Color(const Float ir, const Float ig, const Float ib)
                        : r{ir}, g{ig}, b{ib} {}

        // HSV Constructor
        constexpr Color(const HSV in) : b{in.s}, g{in.s}, r{in.s}
        {
            if (in.s != 0)
            {
            }
        }

        // Grayscale Constructor
        constexpr Color(const Float in) : r{in}, g{in}, b{in} {}

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