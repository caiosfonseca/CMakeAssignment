#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include <iterator>

#include "common/common.h"


CommonLib::CommonLib()
{
    commandList = {"AsyncHello", "SyncedHello", "SendName", "SendAge", "CreatePlayerProfile", "PlayerWalk", "PlayerSleep"};
}

bool CommonLib::CommandExists(const string &value)
{
    return find(commandList.begin(), commandList.end(), value) != commandList.end();
}

string CommonLib::GetCommand(const int &val)
{
    return commandList[val];
}

string CommonLib::GetCommandListAsString()
{
    string result = "";
    for(int i = 0; i < int(commandList.size()); i++)
    {
        result += commandList[i];
        if(i != commandList.size() - 1 )
            result += ", ";
    }
    return result;
}

void CommonLib::ReadConfigFile()
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