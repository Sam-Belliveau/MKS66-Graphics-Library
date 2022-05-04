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
#include "graphics/ZBuffer.hpp"
#include "graphics/drawers/Line.hpp"
#include "graphics/drawers/Triangle.hpp"

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

    private:
        FrameBuffer _scene;
        EdgeListd _edges;
        EdgeListd _triangles;
        std::stack<Mat4d> _transform;

    public:
        Engine(Size x, Size y)
            : _scene{x, y}
            , _edges{}
            , _transform{} 
        {
            for(Size i = 0; i < 64; ++i) 
                _transform.push(Mat4d::Identity());
            
            _scene.add_light(Vertex(Vec3d(0, 1, 0), 0.75 * Color::White));
            _scene.add_light(Vertex(Vec3d(0, -1, -1), 0.75 * Color::Green));
            _scene.add_light(Vertex(Vec3d(1, -1, 1), 0.75 * Color::Red));
            _scene.add_light(Vertex(Vec3d(-1, -1, 1), 0.75 * Color::Blue));
        }

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

            for(Size i = 1; i < _edges.size(); i += 2)
            {
                Line(
                    Vertex(_edges[i - 1]),
                    Vertex(_edges[i - 0])
                )(_scene);
            }

            for(Size i = 2; i < _triangles.size(); i += 3)
            {
                Triangle triangle{
                    Vertex(_triangles[i - 2]),
                    Vertex(_triangles[i - 1]),
                    Vertex(_triangles[i - 0])
                };

                if(view.dot(triangle.normal()) >= 0)
                    triangle(_scene);
            }

            _triangles.clear();
            _edges.clear();
        }

        void add_triangle(const Vec4d& a, const Vec4d& b, const Vec4d& c)
        {
            _triangles.push_back(_transform.top() * a);
            _triangles.push_back(_transform.top() * b);
            _triangles.push_back(_transform.top() * c);
        }

        void add_quad(const Vec4d& a, const Vec4d& b, const Vec4d& c, const Vec4d& d)
        {
            add_triangle(a, b, c);
            add_triangle(c, d, a);
        }

        void add_line(const Vec4d& a, const Vec4d& b)
        {
            _edges.push_back(_transform.top() * a);
            _edges.push_back(_transform.top() * b);
        }

        void add_point(const Vec4d& a)
        {
            add_line(a, a);
        }

    public:
        using CommandT = std::function<void(std::istream& command)>;
        const std::unordered_map<std::string, CommandT> _command_list = 
        {
            {"pop", [&](std::istream& command) {
                if(_transform.size() > 1) _transform.pop();
                else _transform.top() = Mat4d::Identity();
            }},

            {"push", [&](std::istream& command) {
                _transform.push(_transform.top());
            }},

            {"line", [&](std::istream& command) {
                Vec3d a, b;
                command >> a >> b;
                add_line(a, b);
                draw_image();
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
                draw_image();
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
                draw_image();
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
                draw_image();
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

                add_quad(b1, b2, b3, b4);
                add_quad(t4, t3, t2, t1);
                add_quad(b2, b1, t1, t2);
                add_quad(b3, b2, t2, t3);
                add_quad(b4, b3, t3, t4);
                add_quad(b1, b4, t4, t1);
                draw_image();
            }},

            {"sphere", [&](std::istream& command) {
                Vec3d pos;
                Float64 radius;
                command >> pos >> radius;

                const Float64 dp = Math::PI / 128.0;
                const Float64 dt = Math::PI / 64.0;
                for(Float64 phi = 0.0; phi < Math::PI; phi += dp)
                {
                    for(Float64 theta = 0.0; theta <= 2 * Math::PI; theta += dt)
                    {
                        Vec3d da = pos + radius * Vec3d(
                            std::cos(phi),
                            std::sin(phi) * std::cos(theta),
                            std::sin(phi) * std::sin(theta)
                        );

                        Vec3d db = pos + radius * Vec3d(
                            std::cos(phi + dp),
                            std::sin(phi + dp) * std::cos(theta),
                            std::sin(phi + dp) * std::sin(theta)
                        );

                        Vec3d dc = pos + radius * Vec3d(
                            std::cos(phi + dp),
                            std::sin(phi + dp) * std::cos(theta + dt),
                            std::sin(phi + dp) * std::sin(theta + dt)
                        );

                        Vec3d dd = pos + radius * Vec3d(
                            std::cos(phi),
                            std::sin(phi) * std::cos(theta + dt),
                            std::sin(phi) * std::sin(theta + dt)
                        );

                        add_quad(da, db, dc, dd);
                    }
                }
                draw_image();
            }},

            {"torus", [&](std::istream& command) {
                Vec3d pos;
                Float64 radius1;
                Float64 radius2;
                command >> pos >> radius1 >> radius2;

                const Float64 dp = Math::PI / 128.0;
                const Float64 dt = Math::PI / 64.0;
                for(Float64 phi = 0.0; phi <= 2 * Math::PI; phi += dp)
                {
                    for(Float64 theta = 0.0; theta <= 2 * Math::PI; theta += dt)
                    {
                        Vec3d da = pos + Vec3d(
                            radius2 * std::cos(phi) + radius1 * std::cos(phi) * std::cos(theta + dt),
                            radius1 * std::sin(theta + dt),
                            radius2 * std::sin(phi) + radius1 * std::sin(phi) * std::cos(theta + dt)
                        );

                        Vec3d db = pos + Vec3d(
                            radius2 * std::cos(phi + dp) + radius1 * std::cos(phi + dp) * std::cos(theta + dt),
                            radius1 * std::sin(theta + dt),
                            radius2 * std::sin(phi + dp) + radius1 * std::sin(phi + dp) * std::cos(theta + dt)
                        );

                        Vec3d dc = pos + Vec3d(
                            radius2 * std::cos(phi + dp) + radius1 * std::cos(phi + dp) * std::cos(theta),
                            radius1 * std::sin(theta),
                            radius2 * std::sin(phi + dp) + radius1 * std::sin(phi + dp) * std::cos(theta)
                        );

                        Vec3d dd = pos + Vec3d(
                            radius2 * std::cos(phi) + radius1 * std::cos(phi) * std::cos(theta),
                            radius1 * std::sin(theta),
                            radius2 * std::sin(phi) + radius1 * std::sin(phi) * std::cos(theta)
                        );

                        add_quad(da, db, dc, dd);
                    }
                }
                draw_image();
            }},

            {"scale", [&](std::istream& command) {
                Vec3d scale;
                command >> scale;
                _transform.top() = _transform.top() * Mat4d::Scale(scale);
                draw_image();
            }},

            {"move", [&](std::istream& command) {
                Vec3d translate;
                command >> translate;
                _transform.top() = _transform.top() * Mat4d::Translation(translate);
                draw_image();
            }},

            {"rotate", [&](std::istream& command) {
                std::string axis;
                Float64 theta;
                command >> axis >> theta;

                theta = Math::PI * theta / 180.0;

                /**/ if(axis == "x") { _transform.top() = _transform.top() * Mat4d::RotX(theta); }
                else if(axis == "y") { _transform.top() = _transform.top() * Mat4d::RotY(theta); }
                else if(axis == "z") { _transform.top() = _transform.top() * Mat4d::RotZ(theta); }
                else { std::cerr << "Unknown Axis \"" << axis << "\". Ignoring...\n"; }
                draw_image();
            }},

            {"clear_zbuffer", [&](std::istream& command) {
                _scene.zbuffer().clear();
            }},

            {"display", [&](std::istream& command) {
                static Size temp_num = 0;
                std::ofstream file;
                std::string temp_file_name = ".display_tmp_" + std::to_string(temp_num++) + ".ppm";
                file.open(temp_file_name, std::ios::binary);
                file << _scene.image();
                file.close();

                std::system(("open " + temp_file_name).c_str());
            }},

            {"save", [&](std::istream& command) {
                std::string file_name;
                command >> file_name;

                std::string temp_file_name = file_name + ".ppm";

                std::ofstream file;
                file.open(temp_file_name.c_str(), std::ios::binary | std::ios::trunc);
                file << _scene.image();
                file.close();

                std::system(("convert " + temp_file_name + " " + file_name + " && rm -f " + temp_file_name).c_str());
            }},
        };
    };

}