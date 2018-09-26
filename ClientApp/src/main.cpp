#include <iostream>
#include <string>

#include "common/common.h"

int main(int argc, char* argv[])
{
    printf("Hello, CMake Client!\n");

    common cObj;
    
    std::string pipeName = cObj.getPipeNameFromArgs(argc, argv);
    
    return 0;
}