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
constexpr double COUNT = 200;
constexpr double LINE_STEP = 360.0 / COUNT;
constexpr double SCALE = 3.4;
constexpr double CIRCLE_PART = 0.975;

SPGL::Vector2d getCircle(double rad)
{
    return SPGL::Vector2d(
        (WIDTH / SCALE) + CIRCLE_PART * (WIDTH / 3.6) * std::cos(rad), 
        (WIDTH / SCALE) + CIRCLE_PART * (WIDTH / 3.6) * std::sin(rad)
    );
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
            image(x, y) = SPGL::Color::HSV(hue, 0.8, 0.2);
        }
    }

    for(double deg = 360.0; deg >= 0.0; deg -= LINE_STEP)
    {
        double rad = SPGL::Math::PI * deg / 180.0;
        double rad2 = 2 * rad;
        double radNext = rad - SPGL::Math::PI * LINE_STEP / 180.0;
        SPGL::Color color = SPGL::Color::HSV(2.0 * deg, 0.9, 1.0);
        SPGL::Color wcolor = SPGL::Color::HSV(4 * deg, 0.8, 1.0);
        SPGL::FloatingLine(getCircle(rad), getCircle(rad2), wcolor)(image);
        SPGL::FloatingLine(getCircle(rad), getCircle(radNext), color)(image);
    }

    for(double deg = 360.0; deg >= 0.0; deg -= LINE_STEP)
    {
        double rad = SPGL::Math::PI * deg / 180.0;
        double rad2 = 2 * rad;
        double radNext = rad - SPGL::Math::PI * LINE_STEP / 180.0;
        SPGL::Vector2i offset(WIDTH * (SCALE - 2.0) / SCALE, HEIGHT * (SCALE - 2.0) / SCALE);
        SPGL::Color color = SPGL::Color::HSV(2.0 * deg, 0.9, 1.0);
        SPGL::Color wcolor = SPGL::Color::HSV(4 * deg, 0.8, 1.0);
        SPGL::Line<false>(offset + SPGL::Vector2i(getCircle(rad)), offset + SPGL::Vector2i(getCircle(rad2)), wcolor)(image);
        SPGL::Line<false>(offset + SPGL::Vector2i(getCircle(rad)), offset + SPGL::Vector2i(getCircle(radNext)), color)(image);
    }

    std::cerr << "Opening Image File! [" OUTPUT_FILE_NAME "]\n";

    std::ofstream file;
    file.open(OUTPUT_FILE_NAME);
    file << image;

    std::cerr << "Finished Writing!\n";

}