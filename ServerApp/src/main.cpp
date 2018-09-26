#include <iostream>
#include <string>

#include "common/common.h"

int main(int argc, char* argv[])
{
    printf("Hello, CMake Server!\n");

    common cObj;
    
    std::string pipeName = cObj.getPipeNameFromArgs(argc, argv);
  
    return 0;
}

