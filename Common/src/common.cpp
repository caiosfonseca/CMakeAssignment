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

int CommonLib::CommandExists(const string &value)
{
    int iCommand = -1;
    
    try
    {
        iCommand = stoi(value);
        if (iCommand != -1 && iCommand < (int)commandList.size())
            return iCommand;
    }
    catch(const invalid_argument& e)
    {
        vector<string>::iterator position = find(commandList.begin(), commandList.end(), value);
        if (position != commandList.end())
            iCommand = distance(commandList.begin(), position);
            if(iCommand < (int) commandList.size() && iCommand >= 0)
                return iCommand;
    }
    std::cerr << "Invalid command! " << '\n';
    return -1;
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
        result += "(" + to_string(i) + ")";
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

TCHAR* CommonLib::StringToTCHAR(const string &value){
    TCHAR *myTCHAR = new TCHAR[value.size()+1];
    myTCHAR[value.size()] = 0;

    copy(value.begin(), value.end(), myTCHAR);
    return myTCHAR;
}