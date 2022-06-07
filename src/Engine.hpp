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

#include "graphics/Image.hpp"
#include "graphics/ZBuffer.hpp"
#include "graphics/math/Vector2D.hpp"
#include "graphics/math/Vector3D.hpp"
#include "graphics/math/Vector4D.hpp"
#include "graphics/math/Matrix4D.hpp"
#include "graphics/drawers/Line.hpp"
#include "graphics/drawers/Triangle.hpp"
#include "graphics/effects/FXAA.hpp"

#include <iostream>
#include <fstream>

#include <string>
#include <sstream>
#include <unordered_map>
#include <stack>

#include <functional>
#include <chrono>

namespace SPGL
{
    class Engine
    {
    public:
        constexpr static Float kSegments = 1024.0;

    private:
        Color _color;
        FrameBuffer _scene;
        std::stack<Mat4d> _transform;

        int tri_count = 0;

    public:
        Engine(Size x, Size y)
            : _color{Color::White}
            , _scene{x, y}
            , _transform{} 
        { reset(); }

        void reset() 
        {
            for(Size i = 0; i < 64; ++i) 
                _transform.push(Mat4d::Identity());
            _scene.reset();
        }

    public:
        const Mat4d& get_transform() const { return _transform.top(); }
        Mat4d& get_transform() { return _transform.top(); }

        void pop_transform() 
        {
            if(_transform.size() > 1) _transform.pop();
            else get_transform() = Mat4d::Identity();
        }

        void push_transform(const Mat4d& mat)
        { _transform.push(mat); }

        void modify_transform(const Mat4d& mat)
        { get_transform() = get_transform() * mat; }

    public:
        const Image& image() const { return _scene.image(); }

    public:
        void set_material(SYMTAB* constants)
        { _scene.set_material(constants); }

    public:

        void draw_triangle(const Vec4d& a, const Vec4d& b, const Vec4d& c)
        {
            ++tri_count;
            Triangle triangle{ 
                Vertex(get_transform() * a, _color), 
                Vertex(get_transform() * b, _color), 
                Vertex(get_transform() * c, _color) 
            };

            if(_scene.view().dot(triangle.normal()) >= 0)
                triangle(_scene);
        }

        void draw_quad(const Vec4d& a, const Vec4d& b, const Vec4d& c, const Vec4d& d)
        {
            draw_triangle(a, b, c);
            draw_triangle(c, d, a);
        }

        void draw_line(const Vec4d& a, const Vec4d& b)
        {
            Line line{
                Vertex(get_transform() * a, _color), 
                Vertex(get_transform() * b, _color)
            };
            
            line(_scene);
        }

        void draw_point(const Vec4d& a) { draw_line(a, a); }

    public:
        void save(const std::string& file_name)
        {
            std::string temp_file_name = file_name + ".ppm";

            std::ofstream file;
            file.open(temp_file_name.c_str(), std::ios::binary | std::ios::trunc);
            file << FXAA::apply(image());
            file.close();

            std::system(("convert " + temp_file_name + " " + file_name + " && rm -f " + temp_file_name).c_str());
        }
    };

}