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

#include <ostream>
#include "Image.hpp"

namespace SPGL
{
    namespace PPM
    {
        const char* MAGIC_HEADER = "P6\n";
        const Size COLOR_DEPTH = 255;
    }
}

namespace SPGL
{
    std::ostream& operator<<(std::ostream& file, const Image& image)
    {
        file << PPM::MAGIC_HEADER;
        file << image.width() << ' ' << image.height() << '\n';
        file << PPM::COLOR_DEPTH << '\n';
        
        for(const Color& color : image)
        { 
            Color::Bytes raw = color.bytes();
            file.write(reinterpret_cast<const char*>(&raw.r), 1);
            file.write(reinterpret_cast<const char*>(&raw.g), 1);
            file.write(reinterpret_cast<const char*>(&raw.b), 1);
        }
    
        file << '\n';

        return file;
    }
}