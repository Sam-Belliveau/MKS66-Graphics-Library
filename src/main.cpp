#include <iostream>
#include <fstream>

#include <cstdlib>

#include "graphics/Image.hpp"
#include "graphics/PPMSupport.hpp"

int main()
{
    SPGL::Image test(640, 240, SPGL::Color::Red);

    SPGL::UInt8 hue = 128;
    for(SPGL::Color& c : test)
    {
        hue += (rand() % 7) - 3;
        c = SPGL::Color::HSV(hue);
    }

    std::cout << "Opening Image File!\n";

    std::ofstream file;
    file.open("./test_image.ppm");
    file << test;

    std::cout << "Finished Writing!\n";
}