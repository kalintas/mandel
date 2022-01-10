#include <iostream>

#include "mandel.hpp"

int main() {
    if (!mandel::Init()) {
        std::cerr << "couldnt init mandel\n";
        return -1;
    }

    mandel::Run();
}
