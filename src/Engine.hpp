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
#include "graphics/PPMSupport.hpp"
#include "graphics/Vector2D.hpp"
#include "graphics/Vector3D.hpp"
#include "graphics/Vector4D.hpp"
#include "graphics/Matrix4D.hpp"
#include "graphics/drawers/Line.hpp"

#include <iostream>
#include <fstream>

#include <string>
#include <sstream>
#include <unordered_map>

#include <functional>
#include <chrono>

namespace SPGL
{
    class Engine
    {

    private:
        Image _buffer;
        EdgeListd _edges;
        EdgeListd _triangles;
        Mat4d _transform;

    public:
        Engine(Size x, Size y)
            : _buffer{x, y}
            , _edges{}
            , _transform{Mat4d::Identity()} {}

    public:
        bool parse_command(std::istream& stream)
        {
            std::string command;
            if(stream >> command)
            {
                auto function = _command_list.find(command);
                if(function != _command_list.end())
                {
                    std::cerr << "Running [" << command << "]...";
                    function->second(stream);
                    std::cerr << "Finished!\n";
                }

                else if(command[0] == '#' || command[0] == '/')
                { 
                    std::string comment;
                    std::getline(stream, comment);
                    std::cerr << "Comment \"" << command << " " << comment << "\"\n"; 
                }
                else { std::cerr << "Unable To Find Command \"" << command << "\"\n"; }
                std::cerr << "true?\n";
                return true;
            }
            std::cerr << "false?\n";
            return false;
        }

    private:
        void draw_image() 
        {
            const Vec3d view(0, 0, 1);

            std::fill(_buffer.begin(), _buffer.end(), Color::Black);
            for(Size i = 0; i + 1 < _edges.size(); i += 2)
            {
                Vec4d a = _edges[i + 0];
                Vec4d b = _edges[i + 1];
                Line<true> line(Vec2d(a.x, a.y), Vec2d(b.x, b.y), Color::White);
                line(_buffer);
            }

            for(Size i = 0; i + 2 < _triangles.size(); i += 3)
            {
                Vec4d am = _triangles[i + 0];
                Vec4d bm = _triangles[i + 1];
                Vec4d cm = _triangles[i + 2];

                Vec3d a = am, b = bm, c = cm;

                Vec3d p0 = b - a;
                Vec3d p1 = c - a;
                Vec3d n = p0.cross(p1);

                if(view.dot(n) > 0)
                {
                    Line<true>(Vec2d(a.x, a.y), Vec2d(b.x, b.y), Color::White)(_buffer);
                    Line<true>(Vec2d(b.x, b.y), Vec2d(c.x, c.y), Color::White)(_buffer);
                    Line<true>(Vec2d(c.x, c.y), Vec2d(a.x, a.y), Color::White)(_buffer);
                }
            }
        }

        void add_triangle(const Vec4d& a, const Vec4d& b, const Vec4d& c)
        {
            _triangles.push_back(a);
            _triangles.push_back(b);
            _triangles.push_back(c);
        }

        void add_quad(const Vec4d& a, const Vec4d& b, const Vec4d& c, const Vec4d& d)
        {
            add_triangle(a, b, c);
            add_triangle(c, d, a);
        }

        void add_line(const Vec4d& a, const Vec4d& b)
        {
            _edges.push_back(a);
            _edges.push_back(b);
        }

        void add_point(const Vec4d& a)
        {
            add_line(a, a);
        }

    public:
        using CommandT = std::function<void(std::istream& command)>;
        const std::unordered_map<std::string, CommandT> _command_list = 
        {
            {"clear", [&](std::istream& command) {
                _edges.clear();
                _triangles.clear();
            }},

            {"line", [&](std::istream& command) {
                Vec3d a, b;
                command >> a >> b;
                add_line(a, b);
            }},

            {"circle", [&](std::istream& command) {
                Vec3d origin;
                Float64 radius;
                command >> origin >> radius;

                for(Float64 i = 0.0; i < 710.0; i++)
                {
                    Float64 theta_a = (i + 0.0) / 113.0;
                    Float64 theta_b = (i + 1.0) / 113.0;
                    
                    Vec3d point_a = origin + radius * Vec3d(std::cos(theta_a), std::sin(theta_a), 0.0);
                    Vec3d point_b = origin + radius * Vec3d(std::cos(theta_b), std::sin(theta_b), 0.0);
                    add_line(point_a, point_b);
                }
            }},

            {"hermite", [&](std::istream& command) {
                Vec2d a, b, c, d;
                command >> a >> b >> c >> d;

                const Vec2d t3 = 2.0 * a - 2.0 * b + c + d;
                const Vec2d t2 = -3.0 * a + 3.0 * b - 2.0 * c - d;
                const Vec2d t1 = c;
                const Vec2d t0 = a;

                _edges.push_back(Vec3d(a));
                for(Float64 t = 0.0; t < 1.0; t += 1.0 / 256.0)
                {
                    Float64 m = 1.0;
                    Vec2d point = t0 + t1 * (m *= t) + t2 * (m *= t) + t3 * (m *= t);
                    _edges.push_back(Vec3d(point));
                    _edges.push_back(Vec3d(point));
                }
                _edges.push_back(Vec3d(b));
            }},

            {"bezier", [&](std::istream& command) {
                Vec2d a, b, c, d;
                command >> a >> b >> c >> d;

                const Vec2d t3 = -a + 3.0 * b - 3.0 * c + d;
                const Vec2d t2 = 3.0 * a - 6.0 * b + 3.0 * c;
                const Vec2d t1 = -3.0 * a + 3.0 * b;
                const Vec2d t0 = a;

                _edges.push_back(Vec3d(a));
                for(Float64 t = 0.0; t < 1.0; t += 1.0 / 256.0)
                {
                    Float64 m = t;
                    Vec2d point = t0 + t1 * (m *= t) + t2 * (m *= t) + t3 * (m *= t);
                    _edges.push_back(Vec3d(point));
                    _edges.push_back(Vec3d(point));
                }
                _edges.push_back(Vec3d(d));
            }},


            {"box", [&](std::istream& command) {
                Vec3d a, b;
                command >> a >> b;
                b = a + b * Vec3d(+1, -1, -1);

                Vec3d b1(a.x, a.y, a.z);
                Vec3d b2(b.x, a.y, a.z);
                Vec3d b3(b.x, a.y, b.z);
                Vec3d b4(a.x, a.y, b.z);

                Vec3d t1(a.x, b.y, a.z);
                Vec3d t2(b.x, b.y, a.z);
                Vec3d t3(b.x, b.y, b.z);
                Vec3d t4(a.x, b.y, b.z);

                add_quad(b4, b3, b2, b1);
                add_quad(t1, t2, t3, t4);
                add_quad(t2, t1, b1, b2);
                add_quad(t3, t2, b2, b3);
                add_quad(t4, t3, b3, b4);
                add_quad(t1, t4, b4, b1);
            }},

            {"sphere", [&](std::istream& command) {
                Vec3d pos;
                Float64 radius;
                command >> pos >> radius;

                const Float64 dp = Math::PI / 16.0;
                const Float64 dt = Math::PI / 16.0;
                for(Float64 phi = 0.0; phi < Math::PI; phi += dp)
                {
                    for(Float64 theta = 0.0; theta <= 2 * Math::PI; theta += dt)
                    {
                        Vec3d da = radius * Vec3d(
                            std::sin(phi) * std::cos(theta),
                            std::cos(phi),
                            std::sin(phi) * std::sin(theta)
                        );

                        Vec3d db = radius * Vec3d(
                            std::sin(phi + dp) * std::cos(theta),
                            std::cos(phi + dp),
                            std::sin(phi + dp) * std::sin(theta)
                        );

                        Vec3d dc = radius * Vec3d(
                            std::sin(phi + dp) * std::cos(theta + dt),
                            std::cos(phi + dp),
                            std::sin(phi + dp) * std::sin(theta + dt)
                        );

                        Vec3d dd = radius * Vec3d(
                            std::sin(phi) * std::cos(theta + dt),
                            std::cos(phi),
                            std::sin(phi) * std::sin(theta + dt)
                        );

                        add_quad(da, db, dc, dd);
                    }
                }
            }},

            {"torus", [&](std::istream& command) {
                Vec3d pos;
                Float64 radius1;
                Float64 radius2;
                command >> pos >> radius1 >> radius2;

                const Float64 dp = Math::PI / 16.0;
                const Float64 dt = Math::PI / 8.0;
                for(Float64 phi = 0.0; phi <= 2 * Math::PI; phi += dp)
                {
                    for(Float64 theta = 0.0; theta <= 2 * Math::PI; theta += dt)
                    {
                        Vec3d da = Vec3d(
                            radius2 * std::cos(phi) + radius1 * std::cos(phi) * std::cos(theta),
                            radius1 * std::sin(theta),
                            radius2 * std::sin(phi) + radius1 * std::sin(phi) * std::cos(theta)
                        );
 
                        Vec3d db = Vec3d(
                            radius2 * std::cos(phi + dp) + radius1 * std::cos(phi + dp) * std::cos(theta),
                            radius1 * std::sin(theta),
                            radius2 * std::sin(phi + dp) + radius1 * std::sin(phi + dp) * std::cos(theta)
                        );
 
                        Vec3d dc = Vec3d(
                            radius2 * std::cos(phi + dp) + radius1 * std::cos(phi + dp) * std::cos(theta + dt),
                            radius1 * std::sin(theta + dt),
                            radius2 * std::sin(phi + dp) + radius1 * std::sin(phi + dp) * std::cos(theta + dt)
                        );
 
                        Vec3d dd = Vec3d(
                            radius2 * std::cos(phi) + radius1 * std::cos(phi) * std::cos(theta + dt),
                            radius1 * std::sin(theta + dt),
                            radius2 * std::sin(phi) + radius1 * std::sin(phi) * std::cos(theta + dt)
                        );

                        add_quad(da, db, dc, dd);
                    }
                }
            }},

            {"ident", [&](std::istream& command) {
                _transform = Mat4d::Identity();
            }},

            {"scale", [&](std::istream& command) {
                Vec3d scale;
                command >> scale;
                _transform = Mat4d::Scale(scale) * _transform;
            }},

            {"move", [&](std::istream& command) {
                Vec3d translate;
                command >> translate;
                _transform = Mat4d::Translation(translate) * _transform;
            }},

            {"rotate", [&](std::istream& command) {
                std::string axis;
                Float64 theta;
                command >> axis >> theta;

                theta = Math::PI * theta / 180.0;

                /**/ if(axis == "x") { _transform = Mat4d::RotX(theta) * _transform; }
                else if(axis == "y") { _transform = Mat4d::RotY(theta) * _transform; }
                else if(axis == "z") { _transform = Mat4d::RotZ(theta) * _transform; }
                else { std::cerr << "Unknown Axis \"" << axis << "\". Ignoring...\n"; }
            }},

            {"apply", [&](std::istream& command) {
                _edges = _transform * _edges;
                _triangles = _transform * _triangles;
            }},

            {"display", [&](std::istream& command) {
                static Size temp_num = 0;
                std::ofstream file;
                std::string temp_file_name = ".display_tmp_" + std::to_string(temp_num++) + ".ppm";
                file.open(temp_file_name, std::ios::binary);
                draw_image();
                file << _buffer;
                file.close();

                std::system(("open " + temp_file_name).c_str());
            }},

            {"save", [&](std::istream& command) {
                std::string file_name;
                command >> file_name;

                std::string temp_file_name = file_name + ".ppm";

                std::ofstream file;
                file.open(temp_file_name.c_str(), std::ios::binary | std::ios::trunc);
                draw_image();
                file << _buffer;
                file.close();

                std::system(("convert " + temp_file_name + " " + file_name + " && rm -f " + temp_file_name).c_str());
            }},
        };
    };

}