#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>

#include "common/common.h"

using namespace std;

void common::ReadConfigFile()
// Simple routine to read config file and set variables
{
    ifstream inFile;

    inFile.open("..\\..\\..\\config.txt");

    if (!inFile)
    {
        inFile.open("config.txt");
        if(!inFile)
        {
            cerr << "Unable to open file config.txt on project root";
            exit(1);
        }
    }

    string val = "";

    while (inFile >> val)
    {
        cout << "Read :" << val << endl;
        // if (val.substr(0, pipePrefix.size()) == pipePrefix)
        // {
        //     pipeName = val.substr(pipePrefix.size()+1, val.size()-pipePrefix.size()-2);
        //     cout<<pipeName<<endl;
        //     cout << typeid(pipeName).name() << endl;
        // }
    }

    inFile.close();
}