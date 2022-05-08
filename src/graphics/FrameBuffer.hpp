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

#include <vector> // std::vector
#include <algorithm> // std::copy
#include <stdexcept> // std::out_of_range
#include <iterator> // std::reverse_iterator

#include "math/Vector2D.hpp"
#include "TypeNames.hpp"
#include "SkyBox.hpp"
#include "Color.hpp"
#include "ZBuffer.hpp"
#include "Image.hpp"

namespace SPGL
{

    struct FrameBuffer
    {
    public:
        constexpr static Float kA = 0.0;
        constexpr static Float kD = 1.0;
        constexpr static Float kS = 4.0;
        constexpr static Float kSky = 0.25;

    private:
        Image _img;
        ZBuffer _zbuf;
        Vec3d _view;
        SkyBox _sky;
        std::vector<Vertex> _lights;

    public:
        // Default Constructor
        FrameBuffer() {}

        // Copy/Move Constructors
        FrameBuffer(const FrameBuffer& in) = default;
        FrameBuffer(FrameBuffer&& in) = default;
        FrameBuffer& operator=(const FrameBuffer& in) = default;
        FrameBuffer& operator=(FrameBuffer&& in) = default;

        FrameBuffer(Size x, Size y)
            : _img{x, y}
            , _zbuf{x, y}
            , _view{0.0, 0.0, 1.0}
            , _sky{}
            , _lights{} {}

    public:
        const Vec3d& view() const { return _view; }

        void set_view(const Vec3d& view)
        { _view = view; }

        void add_light(const Vertex& light)
        { _lights.push_back(light); }

        void set_sky(SkyBox&& sky) 
        { 
            _sky = sky; 

            const double scale = std::hypot(_img.width(), _img.height());
            for(int x = 0; x < _img.width(); ++x) 
            {
                for(int y = 0; y < _img.height(); ++y) 
                {
                    Vec3d offset {
                        x - double(_img.width() / 2),
                        y - double(_img.height() / 2),
                        0.0
                    };

                    _img(x, y) = _sky(offset - _view * scale);
                }   
            }
        }

        void operator()(const Vertex& p)
        { if(_zbuf.plot(p)) _img(p.pixel()) = p.color(); }

        void operator()(const Vertex& p, const Vec3d& normal)
        { 
            if(_zbuf.plot(p)) 
            {
                Vec3d reflected = (2.0 * normal * (normal.dot(_view)) - _view);
                Color plot = kSky * p.color() * _sky(reflected);
                
                for(const Vertex& light : _lights)
                {
                    Color c = (p.color() * light.color());
                    Vec3d pos = light.pos().normalized();
                    Float cos = std::max(0.0, pos.dot(normal));
                    Float spec = std::max(0.0, (2.0 * normal * cos - pos).normalized().dot(_view));
                    spec *= spec; spec *= spec; spec *= spec; spec *= spec; spec *= spec;
                    
                    plot += kA * c;
                    plot += kD * c * cos;
                    plot += kS * c * spec;
                }
                
                _img(p.pixel()) = plot.clamped(); 
            }
        }

    public:
        ZBuffer& zbuffer() { return _zbuf; }
        const ZBuffer& zbuffer() const { return _zbuf; }

        Image& image() { return _img; }
        const Image& image() const { return _img; }
    };

}