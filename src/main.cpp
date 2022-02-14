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
    SPGL::Image image(WIDTH, HEIGHT, SPGL::Color(0.0));

    SPGL::Vector2i points[] = {
        SPGL::Vector2i(0,0),    SPGL::Vector2i(250, 0),   SPGL::Vector2i(500, 0), 
        SPGL::Vector2i(500,0),  SPGL::Vector2i(500, 250), SPGL::Vector2i(500, 500), 
        SPGL::Vector2i(0,500),  SPGL::Vector2i(250, 500), SPGL::Vector2i(500, 500), 
       SPGL::Vector2i (0,0),    SPGL::Vector2i(0, 250),   SPGL::Vector2i(0, 500)
    };

    for(auto a : points)
    {
        for(auto b : points)
        {
            SPGL::Line<false>(a, b, SPGL::Color::HSV(rand() % 360))(image);
        }
    }

    std::cerr << "Opening Image File! [" OUTPUT_FILE_NAME "]\n";

    std::ofstream file;
    file.open(OUTPUT_FILE_NAME);
    file << image;
    file.close();

    std::cerr << "Finished Writing!\n";

}