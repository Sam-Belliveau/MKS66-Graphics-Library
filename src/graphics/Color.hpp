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

#include <algorithm>
#include <limits>

#include "TypeNames.hpp"
#include "Math.hpp"

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
                : r{Math::floatToByte(in.r)}
                , g{Math::floatToByte(in.g)}
                , b{Math::floatToByte(in.b)} {}

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

        // HSV Constructor
        constexpr static Color HSV(const RepT h, const RepT s = RepT(1.0), const RepT v = RepT(1.0))
        {
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

        // Bytes Constructor
        constexpr Color(const Bytes in) 
            : r{Math::byteToFloat<RepT>(in.r)}
            , g{Math::byteToFloat<RepT>(in.g)}
            , b{Math::byteToFloat<RepT>(in.b)} {}

        // Grayscale Constructor
        constexpr Color(const RepT in) 
            : r{in}, g{in}, b{in} {}

    public: /* Conversion to Byte Type */
        constexpr Bytes bytes() const
        { return Bytes(*this); }

    public: /* Match Function */
        template<class Container>
        Color match(const Container& colors) const
        {
            Color result = Color::Black;
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

    public: /* Variables */
        RepT r, g, b; 

    public: /* Math Operators */
        constexpr RepT distance(const Color other) const
        {
            RepT dr = r - other.r; dr *= dr;
            RepT dg = g - other.g; dg *= dg;
            RepT db = b - other.b; db *= db;
            return dr + dg + db;
        }

        constexpr static Color average(Color lhs, const Color& rhs, const RepT weight = 0.5)
        { return lhs.average(rhs, weight); }
        
        constexpr Color& average(Color rhs, const RepT weight = 0.5)
        { return operator*=(RepT(1.0) - weight).operator+=(rhs *= weight); }

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

        constexpr Color& operator*=(const RepT& rhs)
        {
            r *= rhs; g *= rhs; b *= rhs;
            return clamp();
        }

        constexpr Color& operator/=(const RepT& rhs)
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