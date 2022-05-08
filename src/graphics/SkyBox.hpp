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

#include "Image.hpp"
#include "math/Math.hpp"
#include "math/Vector3D.hpp"
#include <string>
#include <cmath>
#include <fstream>

namespace SPGL
{
    class SkyBox
    {
    private:
        Image _image;

    public:
        SkyBox() : _image{1, 1, Color::Black} {}

        SkyBox(std::string file)
        {
            std::fstream skybox;
            skybox.open(file.c_str(), std::fstream::in);
            skybox >> _image;
            skybox.close();
        }

        SkyBox(const SkyBox& other) = default;
        SkyBox& operator=(const SkyBox& other) = default;
        SkyBox(SkyBox&& other) = default;
        SkyBox& operator=(SkyBox&& other) = default;
        
    public:
        Color operator()(Vec3d dir)
        {
            dir = dir.normalized();

            Float x_ang = std::atan2(dir.z, dir.x);
            return _image.interpolate(Vec2d(
                Math::map(x_ang,    -Math::PI,  Math::PI,   0.0, _image.width() - 1.0), 
                Math::map(dir.y,    -1.0,       1.0,        0.0, _image.height() - 1.0)
            ));
        }
    };
}