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

#include <cstdint>

namespace SPGL
{
    /* Floats */
    using Float80 = long double;
    using Float64 = double;
    using Float32 = float;
    using Float = Float64;

    using FloatMax = Float80;

    /* Integers */
    using Size = std::size_t;

    using UIntMax = std::uintmax_t;
    using IntMax = std::intmax_t;

    using UInt64 = std::uint64_t;
    using Int64 = std::int64_t;

    using UInt32 = std::uint32_t;
    using Int32 = std::int32_t;

    using UInt16 = std::uint16_t;
    using Int16 = std::int16_t;

    using UInt8 = std::uint8_t;
    using Int8 = std::int8_t;
}