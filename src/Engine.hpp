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

                else std::cerr << "Unable To Find Command \"" << command << "\"\n";
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
                Line<true> line(Vec2d(a.x, a.y), Vec2d(b.x, b.y), Color::White);
                line(_buffer);
            }
        }

    public:
        using CommandT = std::function<void(std::istream& command)>;
        const std::unordered_map<std::string, CommandT> _command_list = 
        {
            {"line", [&](std::istream& command) {
                double x0, y0, z0, x1, y1, z1;
                command >> x0 >> y0 >> z0 >> x1 >> y1 >> z1;

                _edges.push_back(Vec3d(x0, y0, z0));
                _edges.push_back(Vec3d(x1, y1, z1));
            }},

            {"ident", [&](std::istream& command) {
                _transform = Mat4d::Identity();
            }},

            {"scale", [&](std::istream& command) {
                double x, y, z;
                command >> x >> y >> z;
                _transform = _transform * Mat4d::Scale(Vec3d(x, y, z));
            }},

            {"move", [&](std::istream& command) {
                double x, y, z;
                command >> x >> y >> z;
                _transform = _transform * Mat4d::Translation(Vec3d(x, y, z));
            }},

            {"rotate", [&](std::istream& command) {
                std::string axis;
                double theta;
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

                std::system(("convert " + temp_file_name + " " + file_name + " && " + "rm -f ./.save_temp.ppm").c_str());
            }},


        };
    };

}