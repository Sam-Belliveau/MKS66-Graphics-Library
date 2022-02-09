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

    for(int i = 50; i < 250; i += 10)
    {
        auto line = SPGL::Line<false>(SPGL::Vector2i(250, 50), SPGL::Vector2i(i, 450), SPGL::Color::Red);
        line(image);
    }
    
    for(int i = 250; i < 450; i += 10)
    {
        auto line = SPGL::Line<true>(SPGL::Vector2i(250, 50), SPGL::Vector2i(i, 450), SPGL::Color::Blue);
        line(image);
    }
    
    for(int x = 0; x < image.width(); ++x)
    {
        for(int y = 0; y < image.width(); ++y)
        {
            SPGL::Color::HSV pixel(x + y, double(y) / image.width(), double(x) / image.height());
            SPGL::Color rgb = pixel;

            for(int i = 0; i < 100; ++i)
            {
                pixel = rgb;
                rgb = pixel;
            }

            image(x, y) = rgb;
        }
    }

    
    std::cerr << "Opening Image File! [" OUTPUT_FILE_NAME "]\n";

    std::ofstream file;
    file.open(OUTPUT_FILE_NAME);
    file << image;

    std::cerr << "Finished Writing!\n";

}