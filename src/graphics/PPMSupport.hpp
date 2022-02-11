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
        const char* MAGIC_HEADER = "P3\n";
        const char* SIZE_HEADER = "\n# Image Size [x, y]\n";
        const char* DEPTH_HEADER = "\n# Color Depth\n";
        const char* DATA_HEADER = "\n# Data [r, g, b]\n";

        const char* LINE_SEPERATOR = "\n";
        const char* COLOR_SEPERATOR = "    ";
        constexpr Size COLOR_SEPERATOR_SIZE = 4;

        constexpr Size COLOR_DEPTH = 255;
        constexpr Size MAX_LINE_SIZE = 70;
    }
}

namespace SPGL
{
    inline std::ostream& write_byte(std::ostream& file, UInt8 byte)
    {
        if(byte < 10)  
            return file << "  " << IntMax(byte);
        if(byte < 100) 
            return file << ' ' << IntMax(byte);
        else 
            return file << IntMax(byte);
    }

    std::ostream& operator<<(std::ostream& file, const Image& image)
    {
        file << PPM::MAGIC_HEADER;

        file << PPM::SIZE_HEADER;
        file << image.width() << ' ' << image.height() << '\n';

        file << PPM::DEPTH_HEADER;
        file << PPM::COLOR_DEPTH << '\n';

        file << PPM::DATA_HEADER;

        Size line_size = 0;
        for(const Color& color : image)
        { 
            Color::Bytes raw = color.bytes();
            line_size += 4; write_byte(file, raw.r) << ' ';
            line_size += 4; write_byte(file, raw.g) << ' ';
            line_size += 3; write_byte(file, raw.b);

            if(line_size < PPM::MAX_LINE_SIZE)
            {
                file << PPM::COLOR_SEPERATOR;
                line_size += PPM::COLOR_SEPERATOR_SIZE;
            }

            else { file << '\n'; line_size = 0; }
        }
    
        file << '\n';

        return file;
    }
}