/*** FULL SOURCE CODE @ https://github.com/Sam-Belliveau/MKS66-Graphics-Library ***/

#include <iostream>
#include <fstream>

#include <cstdlib>

#include <complex>

#include "graphics/drawers/Line.hpp"
#include "graphics/MathConstants.hpp"
#include "graphics/Image.hpp"
#include "graphics/PPMSupport.hpp"
#include "graphics/Matrix4D.hpp"

#define OUTPUT_FILE_NAME "output_image.ppm"

constexpr SPGL::Size WIDTH = 240;
constexpr SPGL::Size HEIGHT = 180;
constexpr double COUNT = 256;
constexpr double LINE_STEP = 360.0 / COUNT;
constexpr double SCALE = 3.5;
constexpr double CIRCLE_PART = 0.975;

SPGL::Vec2d getCircle(SPGL::Vec2d offset, double rad)
{
    return offset + SPGL::Vec2d(
        (WIDTH / SCALE) + CIRCLE_PART * (WIDTH / SCALE) * std::cos(rad), 
        (WIDTH / SCALE) + CIRCLE_PART * (WIDTH / SCALE) * std::sin(rad)
    );
}

int main()
{ 
    SPGL::Mat4d test_a = SPGL::Mat4d::Identity();
    SPGL::Mat4d test_b(
        {1, 4, 7, 10},
        {2, 5, 8, 11},
        {3, 6, 9, 12},
        {1, 1, 1, 1}
    );
    
    SPGL::EdgeList<double> list;
    list.push_back(SPGL::Vec3d(1, 2, 3));
    list.push_back(SPGL::Vec3d(4, 5, 6));

    std::cout << "Identity Matrix:\n";
    std::cout << list << std::endl;
    std::cout << test_a << std::endl;
    std::cout << test_a * list << std::endl;

    std::cout << "Wacky Matrix:\n";
    std::cout << list << std::endl;
    std::cout << test_b << std::endl;
    std::cout << test_b * list << std::endl;
    
    return 0;
}