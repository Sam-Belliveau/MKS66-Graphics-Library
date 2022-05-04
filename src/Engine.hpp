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
    public:
        constexpr static Float kSegments = 512.0;

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
        {
            for(Size i = 0; i < 64; ++i) 
                _transform.push(Mat4d::Identity());
            
            _scene.add_light(Vertex(Vec3d(0, 1, 0), 0.8 * Color::White));
            _scene.add_light(Vertex(Vec3d(0, -1, -1), 0.2 * Color::Green));
            _scene.add_light(Vertex(Vec3d(1, -1, 1), 0.2 * Color::Red));
            _scene.add_light(Vertex(Vec3d(-1, -1, 1), 0.2 * Color::Blue));
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
        Mat4d& get_transform() { return _transform.top(); }

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
        using CommandT = std::function<void(std::istream& command)>;
        const std::unordered_map<std::string, CommandT> _command_list = 
        {
            {"pop", [&](std::istream& command) {
                if(_transform.size() > 1) _transform.pop();
                else get_transform() = Mat4d::Identity();
            }},

            {"push", [&](std::istream& command) {
                _transform.push(get_transform());
            }},

            {"color", [&](std::istream& command) {
                Color color;
                command >> color.r >> color.g >> color.b;

                _color = color;
            }},

            {"line", [&](std::istream& command) {
                Vec3d a, b;
                command >> a >> b;
                draw_line(a, b);
            }},

            {"circle", [&](std::istream& command) {
                Vec3d origin;
                Float64 radius;
                command >> origin >> radius;

                Vec3d last_point = origin + radius * Vec3d(1.0, 0.0, 0.0);

                const Float dt = Math::PI / kSegments;
                for(Float theta = dt; theta < Math::TAU; theta += dt)
                {
                    Vec3d point = origin + radius * Vec3d(std::cos(theta), std::sin(theta), 0.0);
                    draw_line(last_point, point);
                    last_point = point;
                }
            }},

            {"hermite", [&](std::istream& command) {
                Vec2d a, b, c, d;
                command >> a >> b >> c >> d;

                const Vec2d t3 = 2.0 * a - 2.0 * b + c + d;
                const Vec2d t2 = -3.0 * a + 3.0 * b - 2.0 * c - d;
                const Vec2d t1 = c;
                const Vec2d t0 = a;

                Vec2d last_point = t0;
                for(Float64 t = 0.0; t < 1.0; t += 1.0 / kSegments)
                {
                    Float64 m = 1.0;
                    Vec2d point = t0;
                    point += t1 * (m *= t);
                    point += t2 * (m *= t);
                    point += t3 * (m *= t);
                    draw_line(Vec3d(last_point), Vec3d(point));
                    last_point = point;
                }
            }},

            {"bezier", [&](std::istream& command) {
                Vec2d a, b, c, d;
                command >> a >> b >> c >> d;

                const Vec2d t3 = -a + 3.0 * b - 3.0 * c + d;
                const Vec2d t2 = 3.0 * a - 6.0 * b + 3.0 * c;
                const Vec2d t1 = -3.0 * a + 3.0 * b;
                const Vec2d t0 = a;

                Vec2d last_point = t0;
                for(Float64 t = 0.0; t < 1.0; t += 1.0 / kSegments)
                {
                    Float64 m = t;
                    Vec2d point = t0;
                    point += t1 * (m *= t);
                    point += t2 * (m *= t);
                    point += t3 * (m *= t);
                    draw_line(Vec3d(last_point), Vec3d(point));
                    last_point = point;
                }
            }},


            {"box", [&](std::istream& command) {
                Vec3d a, b;
                command >> a >> b;
                b = a + b * Vec3d(+1, -1, -1);

                Vec3d b1(a.x, a.y, a.z), b2(b.x, a.y, a.z), b3(b.x, a.y, b.z), b4(a.x, a.y, b.z);
                Vec3d t1(a.x, b.y, a.z), t2(b.x, b.y, a.z), t3(b.x, b.y, b.z), t4(a.x, b.y, b.z);

                draw_quad(b1, b2, b3, b4); draw_quad(t4, t3, t2, t1);
                draw_quad(b2, b1, t1, t2); draw_quad(b3, b2, t2, t3);
                draw_quad(b4, b3, t3, t4); draw_quad(b1, b4, t4, t1);
            }},

            {"sphere", [&](std::istream& command) {
                Vec3d pos;
                Float64 radius;
                command >> pos >> radius;

                const Float64 dp = Math::PI / kSegments;
                const Float64 dt = Math::PI / kSegments;
                for(Float64 phi = 0.0; phi < Math::PI; phi += dp)
                {
                    for(Float64 theta = 0.0; theta <= Math::TAU; theta += dt)
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

                        draw_quad(da, db, dc, dd);
                    }
                }
            }},

            {"torus", [&](std::istream& command) {
                Vec3d pos;
                Float64 radius1;
                Float64 radius2;
                command >> pos >> radius1 >> radius2;

                const Float64 dp = Math::TAU / kSegments;
                const Float64 dt = Math::TAU / kSegments;
                for(Float64 phi = 0.0; phi <= Math::TAU; phi += dp)
                {
                    for(Float64 theta = 0.0; theta <= Math::TAU; theta += dt)
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

                        draw_quad(da, db, dc, dd);
                    }
                }
            }},

            {"scale", [&](std::istream& command) {
                Vec3d scale;
                command >> scale;
                get_transform() = get_transform() * Mat4d::Scale(scale);
            }},

            {"move", [&](std::istream& command) {
                Vec3d translate;
                command >> translate;
                get_transform() = get_transform() * Mat4d::Translation(translate);
            }},

            {"rotate", [&](std::istream& command) {
                std::string axis;
                Float64 theta;
                command >> axis >> theta;

                theta = Math::PI * theta / 180.0;

                /**/ if(axis == "x") { get_transform() = get_transform() * Mat4d::RotX(theta); }
                else if(axis == "y") { get_transform() = get_transform() * Mat4d::RotY(theta); }
                else if(axis == "z") { get_transform() = get_transform() * Mat4d::RotZ(theta); }
                else { std::cerr << "Unknown Axis \"" << axis << "\". Ignoring...\n"; }
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

                std::cerr << "# of Triangles: " << tri_count << "\n";

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