#include <iostream>
#include <string>

#include "common/common.h"

std::string common::getPipeNameFromArgs(int argc, char* argv[])
{
    std::string pipePrefix = "-pipe=";
    std::string argument = "";
    std::string pipeName = "Assignment";

    for (int i = 1; i < argc; ++i) 
    {
        argument = argv[i];
        // std::cout << "Argv[ " << i << "] = " << argument << std::endl; 
        if (argument.substr(0, pipePrefix.size()) == pipePrefix)
        {
            pipeName = argument.substr(pipePrefix.size());
        }
    }

    pipeName = "\\\\.\\pipe\\"+pipeName;
    std::cout << "Pipe name will be " << pipeName << std::endl;
    return pipeName;
}