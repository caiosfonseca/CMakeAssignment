#include <iostream>
#include <string>
#include <tchar.h>

#include "common/common.h"

bool SyncedMessage(TCHAR* command, const TCHAR *args[]);

int _tmain(int argc, TCHAR *argv[])
{
    printf("Hello, CMake Client!\n");
    LPTSTR lpszWrite = TEXT("Default message");
    char command[BUFFER_SIZE*sizeof(TCHAR)];
    char quit[] = "Exit";

    CommonLib cObj;
    string availableCommands = cObj.GetCommandListAsString();
    BOOL bSuccess;
    int commandIndex;

    do {
        //Get client's desired command
        cout << "Choose a command from: " << availableCommands << ", " << quit << endl;
        cin.getline(command, BUFFER_SIZE);

        bSuccess = FALSE;
        commandIndex = cObj.CommandExists(command);
        
        if (commandIndex >= 0)
        {
            if(commandIndex == 0)//AsyncedHello
            {
                cout << "Asynced Hello" << endl;
            }
            else if(commandIndex == 1)//SyncedHello
            {
                cout << "Synced Hello" << endl;
                bSuccess = SyncedMessage(cObj.StringToTCHAR(cObj.GetCommand(1)), {});
                cout << "Success = " << bSuccess << endl;
            }
            else if(commandIndex == 2)//SendName
            {
                cout << "Send Name" << endl;
            }
            else if(commandIndex == 3)//SendAge
            {
                cout << "Send Age" << endl;
            }
            else if(commandIndex == 4)//CreatePlayerProfile
            {
                cout << "Create Player Profile" << endl;
            }
            else if(commandIndex == 5)//PlayerWalk
            {
                cout << "Player Walk" << endl;
            }
            else if(commandIndex == 6)//PlayerSleep
            {
                cout << "Player Sleep" << endl;
            }
            //Execute the appropriate command
        }
        
        
    } while (strcmp (command,quit) != 0);  

    cout << "Quitting client" << endl;  
    
    return 0;
}

bool SyncedMessage(TCHAR* command, const TCHAR *args[]){
    
    bool success = false;
    TCHAR chReadBuf[BUFFER_SIZE*sizeof(TCHAR)]; 

    TCHAR* treatedArgs = "";

    LPTSTR lpszMessage = TEXT("");
    CommonLib cObj;

    string test = command;
    if (lstrlen(treatedArgs) > 0)
        test += string(" "), treatedArgs;
    lpszMessage = cObj.StringToTCHAR(test);
    
    DWORD cbRead; 
    
    success = CallNamedPipe( 
        PIPE_NAME,        // pipe name 
        lpszMessage,           // message to server 
        (lstrlen(lpszMessage)+1)*sizeof(TCHAR), // message length 
        chReadBuf,              // buffer to receive reply 
        BUFFER_SIZE*sizeof(TCHAR),  // size of read buffer 
        &cbRead,                // number of bytes read 
        PIPE_TIMEOUT
    );                 // waits for 20 seconds 
 
    if (success || GetLastError() == ERROR_MORE_DATA) 
    { 
        _tprintf( TEXT("%s\n"), chReadBuf ); 
        
        // The pipe is closed; no more data can be read. 
    
        if (! success) 
        {
            printf("\nExtra data in message was lost\n"); 
        }
    } 
    return success;
}