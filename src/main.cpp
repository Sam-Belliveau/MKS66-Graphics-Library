/*** FULL SOURCE CODE @ https://github.com/Sam-Belliveau/MKS66-Graphics-Library ***/

#include <iostream>
#include <fstream>

#include <cstdlib>

#include <complex>

#include "graphics/drawers/Line.hpp"
#include "graphics/MathConstants.hpp"
#include "graphics/Image.hpp"
#include "graphics/PPMSupport.hpp"

#define OUTPUT_FILE_NAME "output_image.ppm"

constexpr SPGL::Size WIDTH = 64;
constexpr SPGL::Size HEIGHT = 46;


int main()
{
    SPGL::Image image(WIDTH, HEIGHT, SPGL::Color::Black);

    auto line = SPGL::BresenhamLine(SPGL::Vector2i(10, 10), SPGL::Vector2i(30, 20), SPGL::Color::White);
    line(image);
    
    std::cerr << "Opening Image File! [" OUTPUT_FILE_NAME "]\n";

    std::ofstream file;
    file.open(OUTPUT_FILE_NAME);
    file << image;

    std::cerr << "Finished Writing!\n";

}