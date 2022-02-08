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

#ifndef SPGL_IMAGE_HPP
#define SPGL_IMAGE_HPP 1

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
        Vector2D<Size> vecsize() const { return img_size; }
        Size height() const { return img_size.y; }
        Size width()  const { return img_size.x; }
        bool empty()  const { return (img_size.x | img_size.y) == 0; }
        Size size()   const { return img_size.x * img_size.y; }

        const value_type* data() const { return img_data.data(); }
        value_type* data() { return img_data.data(); }

    public: /* Constructors */
        // Default Constructor
        Image() {}

        // Copy/Move Constructors
        Image(const Image& in) = default;
        Image(Image&& in) = default;
        Image& operator=(const Image& in) = default;
        Image& operator=(Image&& in) = default;

        // Create Functions
        Image(Size x, Size y, Color color = Color(0,0,0)) { create(x, y, color); }
        Image& create(Size x, Size y, Color color = Color(0,0,0)) 
        {
            img_size = Vector2s(x, y);
            img_data.resize(x*y);
            std::fill(img_data.begin(), img_data.end(), color);
            return *this;
        }

    public: /* Functions */
        // Single value indexing
        value_type& operator[](Size i)
        { return img_data[i]; }

        const value_type& operator[](Size i) const 
        { return img_data[i]; }

        value_type& operator()(Size i) 
        { return img_data[i]; }

        const value_type& operator()(Size i) const 
        { return img_data[i]; }

        // Double value indexing (unsafe)
        value_type& operator()(Size inX, Size inY) 
        { return img_data[inY*width() + inX]; }

        const value_type& operator()(Size inX, Size inY) const 
        { return img_data[inY*width() + inX]; }

        // Double value indexing (safe)
        value_type& getPixel(Size inX, Size inY)
        {
            if(inX > width())  throw std::out_of_range("X Too Large!");
            if(inY > height()) throw std::out_of_range("Y Too Large!");
            return img_data[inY*width() + inX];
        }

        const value_type& getPixel(Size inX, Size inY) const
        {
            if(inX > width())  throw std::out_of_range("X Too Large!");
            if(inY > height()) throw std::out_of_range("Y Too Large!");
            return img_data[inY*width() + inX];
        }

        // Vector indexing (unsafe)
        value_type& operator[](Vector2s i)
        { return operator()(i.x, i.y); }

        const value_type& operator[](Vector2s i) const 
        { return operator()(i.x, i.y); }

        value_type& operator()(Vector2s i)
        { return operator()(i.x, i.y); }

        const value_type& operator()(Vector2s i) const 
        { return operator()(i.x, i.y); }

        // Vector indexing (unsafe)
        value_type& getPixel(Vector2s i)
        { return getPixel(i.x, i.y); }

        const value_type& getPixel(Vector2s i) const
        { return getPixel(i.x, i.y); }

    private: /* Raw Data */
        std::vector<value_type> img_data;
        Vector2D<Size> img_size;

    public: /* Iterators */
        auto begin() { return std::begin(img_data); }
        auto begin() const { return std::cbegin(img_data); }
        auto cbegin() const { return std::cbegin(img_data); }

        auto end() { return std::end(img_data); }
        auto end() const { return std::cend(img_data); }
        auto cend() const { return std::cend(img_data); }

        auto rbegin() { return std::end(img_data); }
        auto rbegin() const { return std::cend(img_data); }
        auto crbegin() const { return std::cend(img_data); }

        auto rend() { return std::begin(img_data); }
        auto rend() const { return std::cbegin(img_data); }
        auto crend() const { return std::cbegin(img_data); }
    };
}

#endif // SPGL_IMAGE_HPP