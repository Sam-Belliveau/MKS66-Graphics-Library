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

void line(SPGL::Image& img, SPGL::Vec2d start, SPGL::Vec2d end, SPGL::Color color) {
    start = -start;
    end = -end;
    start += {1, 1};
    start *= img.width() / 2;
    end += {1, 1};
    end *= img.width() / 2;
    SPGL::Line<true>(start, end, color)(img);
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

    SPGL::Image out(500, 500);

    SPGL::EdgeList<double> a;

    for(double x = -1; x <= 1; x += 2)
    for(double y = -1; y <= 1; y += 2)
    for(double z = -1; z <= 1; z += 2)
        a.push_back({x, y, z});

    SPGL::Mat4d ta = SPGL::Mat4d::Pitch(-0.2);
    SPGL::Mat4d tb = SPGL::Mat4d::Yaw(0.5);
    SPGL::Mat4d tc = SPGL::Mat4d::Roll(1.5);
    SPGL::Mat4d b = SPGL::Mat4d::Translation({0, 0, 3});

    SPGL::EdgeList<double> at = b * tc * tb * ta * a;

    for(int ka = 0; ka < at.size(); ++ka)
    for(int kb = ka+1; kb < at.size(); ++kb) {
        line(out, SPGL::Vec3d(at[ka]), SPGL::Vec3d(at[kb]), SPGL::Color::HSV(60 * ka + 30 * kb, 0.5, ((1.0/at[ka].z)+(1.0/at[kb].z))));
    }

    std::ofstream file;
    file.open(OUTPUT_FILE_NAME);

    file << out;

    file.close();

    return 0;
}