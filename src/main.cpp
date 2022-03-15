/*** FULL SOURCE CODE @ https://github.com/Sam-Belliveau/MKS66-Graphics-Library ***/

#include <iostream>
#include "Engine.hpp"

int main()
{
    std::string line;
    std::ifstream file;
    file.open("./script.txt");

    SPGL::Engine engine(500, 500);
        std::cerr << "AHHH\n";
    while(engine.parse_command(file)) {
        std::cerr << "AHHH\n";
    }

    return 0;
}