#include <iostream>
#include "RathalosEngine.hpp"

int start(int ac, char** av) {
    re::Window window("Rathalos Engine", 1280, 720);
    re::RathalosEngine engine;

    while (window.open()) {
        window.pollEvents();
    }

    return 0;
}

int main(int ac, char **av) {
    
    try {
        return start(ac, av);
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 1;
}