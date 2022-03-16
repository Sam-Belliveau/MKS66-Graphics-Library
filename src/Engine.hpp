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
                { std::cerr << "Comment \"" << command << "\"\n"; }
                else { std::cerr << "Unable To Find Command \"" << command << "\"\n"; }
                return true;
            }
            return false;
        }

    private:
        void draw_image() 
        {
            std::fill(_buffer.begin(), _buffer.end(), Color::Black);
            for(Size i = 0; i < _edges.size() - 1; i += 2)
            {
                Vec4d a = _edges[i + 0];
                Vec4d b = _edges[i + 1];
                Line<false> line(Vec2d(a.x, a.y), Vec2d(b.x, b.y), Color::White);
                line(_buffer);
            }
        }

        void add_line(const Vec4d& a, const Vec4d& b)
        {
            _edges.push_back(a);
            _edges.push_back(b);
        }

    public:
        using CommandT = std::function<void(std::istream& command)>;
        const std::unordered_map<std::string, CommandT> _command_list = 
        {
            {"line", [&](std::istream& command) {
                Vec3d a, b;
                command >> a >> b;
                add_line(a, b);
            }},

            {"circle", [&](std::istream& command) {
                Vec3d origin;
                Float64 radius;
                command >> origin >> radius;

                for(Float64 i = 0.0; i < 2.0 * 355.0; ++i)
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

                for(Float64 t = 0.0; t <= 1.0; t += 1.0 / 256.0)
                {
                    Float64 m = t;
                    Vec2d point = t0;
                    point += t1 * m; m *= t;
                    point += t2 * m; m *= t;
                    point += t3 * m;
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

                for(Float64 t = 0.0; t <= 1.0; t += 1.0 / 256.0)
                {
                    Float64 m = t;
                    Vec2d point = t0;
                    point += t1 * m; m *= t;
                    point += t2 * m; m *= t;
                    point += t3 * m;
                    _edges.push_back(Vec3d(point));
                    _edges.push_back(Vec3d(point));
                }

                _edges.push_back(Vec3d(d));
            }},


            {"ident", [&](std::istream& command) {
                _transform = Mat4d::Identity();
            }},

            {"scale", [&](std::istream& command) {
                Vec3d scale;
                command >> scale;
                _transform = _transform * Mat4d::Scale(scale);
            }},

            {"move", [&](std::istream& command) {
                Vec3d translate;
                command >> translate;
                _transform = _transform * Mat4d::Translation(translate);
            }},

            {"rotate", [&](std::istream& command) {
                std::string axis;
                Float64 theta;
                command >> axis >> theta;

                theta = 355.0 * theta / 20340.0;

                /**/ if(axis == "x") { _transform = _transform * Mat4d::RotX(theta); }
                else if(axis == "y") { _transform = _transform * Mat4d::RotY(theta); }
                else if(axis == "z") { _transform = _transform * Mat4d::RotZ(theta); }
                else { std::cerr << "Unknown Axis \"" << axis << "\". Ignoring...\n"; }
            }},

            {"apply", [&](std::istream& command) {
                _edges = _transform * _edges;
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

                std::system(("convert " + temp_file_name + " " + file_name + " && " + "rm -f " + temp_file_name).c_str());
            }},


        };
    };

}