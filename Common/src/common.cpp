#include <iostream>
#include <string>
#include <windows.h>
#include <iterator>

#include "common/common.h"


CommonLib::CommonLib()
{
    commandList = {"AsyncHello", "SyncedHello", "SendName", "SendAge", "CreateUserProfile", "UserBirthday", "FindUserProfile", "GetUserProfile"};
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
        (void)e;
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

TCHAR* CommonLib::StringToTCHAR(const string &value){
    TCHAR *myTCHAR = new TCHAR[value.size()+1];
    myTCHAR[value.size()] = 0;

    copy(value.begin(), value.end(), myTCHAR);
    return myTCHAR;
}