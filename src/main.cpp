#include "mandel.h"

int main()
{   

    if(!mandel::Init()) return -1;
    
    mandel::Run();

    return 0;
}
