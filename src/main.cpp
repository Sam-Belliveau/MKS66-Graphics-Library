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

SPGL::Vector2i getCircle(double rad)
{
    return SPGL::Vector2i(std::lround(250 + 225 * std::cos(rad)), std::lround(250 + 225 * std::sin(rad)));
}

int main()
{
    SPGL::Image image(WIDTH, HEIGHT, SPGL::Color::Black);

    for(int x = 0; x < image.width(); ++x)
    {
        for(int y = 0; y < image.height(); ++y)
        {
            double hue = y;
            hue += 30 * std::sin(x / 25.0); 
            hue += 30 * std::cos(y / 25.0); 
            image(x, y) = SPGL::Color::HSV(hue, 0.7, 0.3);
        }
    }

    for(double deg = 360.0; deg >= 0.0; deg -= 1.8)
    {
        double rad = SPGL::Math::PI * deg / 180.0;
        double rad2 = 2 * rad;
        double radNext = rad - SPGL::Math::PI * 1.8 / 180.0;
        SPGL::Color color = SPGL::Color::HSV(deg, 0.75, 1.0);
        SPGL::Line<true>(getCircle(rad), getCircle(rad2), color)(image);
        SPGL::Line<false>(getCircle(rad), getCircle(radNext), color)(image);
    }

    std::cerr << "Opening Image File! [" OUTPUT_FILE_NAME "]\n";

    std::ofstream file;
    file.open(OUTPUT_FILE_NAME);
    file << image;

    std::cerr << "Finished Writing!\n";

}