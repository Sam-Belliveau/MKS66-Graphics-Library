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

constexpr SPGL::Size WIDTH = 500;
constexpr SPGL::Size HEIGHT = 500;


int main()
{
    SPGL::Image image(WIDTH, HEIGHT, SPGL::Color::Black);

    auto line = SPGL::Line<false>(SPGL::Vector2i(100, 100), SPGL::Vector2i(300, 200), SPGL::Color::Red);
    line(image);

    line = SPGL::Line(SPGL::Vector2i(100, 100), SPGL::Vector2i(200, 300), SPGL::Color::Blue);
    line(image);
    
    line = SPGL::Line(SPGL::Vector2i(200, 300), SPGL::Vector2i(300, 200), SPGL::Color::Green);
    line(image);
    
    std::cerr << "Opening Image File! [" OUTPUT_FILE_NAME "]\n";

    std::ofstream file;
    file.open(OUTPUT_FILE_NAME);
    file << image;

    std::cerr << "Finished Writing!\n";

}