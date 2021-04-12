#include "mandel.h"

#include <iostream>

#include <ImGui/imgui.h>

int main()
{   

    if(!mandel::Init())
    {
        std::cerr << "couldnt init mandel\n";
        return -1;
    }

    mandel::Run();

    return 0;
}
