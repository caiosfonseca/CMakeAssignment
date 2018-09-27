#include <iostream>
#include <string>
#include <tchar.h>

#include "common/common.h"

int _tmain(int argc, TCHAR *argv[])
{
    printf("Hello, CMake Client!\n");
    LPTSTR lpszWrite = TEXT("Default message");
    char command[BUFFER_SIZE*sizeof(TCHAR)];
    char quit[] = "Exit";
    HANDLE hPipe;

    CommonLib cObj;
    string availableCommands = cObj.GetCommandListAsString();
    BOOL bSuccess;

    do {
        //Get client's desired command
        cout << "Choose a command from: " << availableCommands << ", " << quit << endl;
        cin.getline(command, BUFFER_SIZE);

        bSuccess = FALSE;
        
        if (cObj.CommandExists(command))
        {
            if(command == cObj.GetCommand(0))//AsyncedHello
            {
                cout << "Asynced Hello" << endl;
            }
            else if(command == cObj.GetCommand(1))//SyncedHello
            {
                cout << "Synced Hello" << endl;
            }
            else if(command == cObj.GetCommand(2))//SendName
            {
                cout << "Send Name" << endl;
            }
            else if(command == cObj.GetCommand(3))//SendAge
            {
                cout << "Send Age" << endl;
            }
            else if(command == cObj.GetCommand(4))//CreatePlayerProfile
            {
                cout << "Create Player Profile" << endl;
            }
            else if(command == cObj.GetCommand(5))//PlayerWalk
            {
                cout << "Player Walk" << endl;
            }
            else if(command == cObj.GetCommand(6))//PlayerSleep
            {
                cout << "Player Sleep" << endl;
            }
            //Execute the appropriate command
        }
        
        
    } while (strcmp (command,quit) != 0);  

    cout << "Quitting client" << endl;  
    
    return 0;
}