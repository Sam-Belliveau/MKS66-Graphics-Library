/*** FULL SOURCE CODE @ https://github.com/Sam-Belliveau/MKS66-Graphics-Library ***/

#include <iostream>
#include <fstream>

#include <cstdlib>

#include <complex>

#include "graphics/MathConstants.hpp"
#include "graphics/Image.hpp"
#include "graphics/PPMSupport.hpp"

#define OUTPUT_FILE_NAME "output_image.ppm"

constexpr SPGL::Size WIDTH = 1080;
constexpr SPGL::Size HEIGHT = 1080;

constexpr SPGL::Size MAX_ITER = 512;

int getIterations(int width, int height);

int main()
{
    SPGL::Image image(WIDTH, HEIGHT, SPGL::Color::Red);

    SPGL::UInt8 hue = 128;
    for(int x = 0; x < image.width(); ++x)
    {
        for(int y = 0; y < image.width(); ++y)
        {
            int iter = getIterations(x, y);

            if(iter < 0)
            {
                image(x, y) = SPGL::Color::Black;
            }

            else
            {
                image(x, y) = SPGL::Color::HSV(SPGL::UInt8(iter));
            }
        }   
    }

    std::cout << "Opening Image File! [" OUTPUT_FILE_NAME "]\n";

    std::ofstream file;
    file.open(OUTPUT_FILE_NAME);
    file << image;

    std::cout << "Finished Writing!\n";
}

using Complex = std::complex<double>;
Complex getRotation(double deg)
{
    double rad = deg * SPGL::Math::PI / 180.0;
    return Complex(std::cos(rad), std::sin(rad));
}

int getIterations(int x, int y)
{
    Complex z, c(2.5 * (x / double(WIDTH)) - 1.0, 2.5 * (y / double(HEIGHT)) - 1.5);
    c = (1.0 / (2.0 * c * getRotation(45)));
    z = 0;

    for(int iter = 0; iter < MAX_ITER; ++iter) 
    {
        z = z * z + c;

        if(std::abs(z) > 2.0)
            return iter;
    }

    return -1;
}