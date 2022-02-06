/*** FULL SOURCE CODE @ https://github.com/Sam-Belliveau/MKS66-Graphics-Library ***/

#include <iostream>
#include <fstream>

#include <cstdlib>

#include "graphics/Image.hpp"
#include "graphics/PPMSupport.hpp"

int main()
{
    SPGL::Image image(512, 512, SPGL::Color::Red);

    SPGL::UInt8 hue = 128;
    for(SPGL::Color& c : image)
    {
        hue += (rand() % 3) - 1;
        c = SPGL::Color::HSV(hue);
    }

    std::cout << "Opening Image File!\n";

    std::ofstream file;
    file.open("./test_image.ppm");
    file << image;

    std::cout << "Finished Writing!\n";
}