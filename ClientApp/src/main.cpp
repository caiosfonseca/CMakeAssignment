#include <iostream>
#include <string>
#include <tchar.h>

#include "common/common.h"

// Client pipe based of Microsoft's
// https://docs.microsoft.com/en-us/windows/desktop/ipc/named-pipe-client
// https://docs.microsoft.com/en-us/windows/desktop/ipc/transactions-on-named-pipes

struct CommandAndArgs
{
    TCHAR* command;
    vector<TCHAR*> args;
};

bool SyncedMessage(TCHAR* command, vector<TCHAR*> args);
bool ASyncedMessage(TCHAR* command, vector<TCHAR*> args);
DWORD WINAPI InstanceThread(LPVOID);

int _tmain(int argc, TCHAR *argv[])
{
    printf("Hello, CMake Client!\n");
    LPTSTR lpszWrite = TEXT("Default message");
    char command[PIPE_BUFFER_SIZE];
    char quit[] = "Exit";

    CommonLib cObj;
    string availableCommands = cObj.GetCommandListAsString();
    BOOL bSuccess;
    int commandIndex;

    do {
        //Get client's desired command
        cout << "Choose a command from: " << availableCommands << ", " << quit << endl;
        cin.getline(command, BUFFER_SIZE);
        if (strcmp (command,quit) == 0)
            return 0;

        bSuccess = FALSE;
        commandIndex = cObj.CommandExists(command);
        
        if (commandIndex >= 0)
        {
            cout << "Client Running: " ;
            switch(commandIndex){
                case 0:
                    cout << "Asynced Hello" << endl;
                    bSuccess = ASyncedMessage(cObj.StringToTCHAR(cObj.GetCommand(commandIndex)), {});
                    break;
                case 1:
                    cout << "Synced Hello" << endl;
                    bSuccess = SyncedMessage(cObj.StringToTCHAR(cObj.GetCommand(commandIndex)), {});
                    break;
                case 2:
                    cout << "Send Name" << endl;
                    break;
                case 3:
                    cout << "Send Age" << endl;
                    break;
                case 4:
                    cout << "Create Player Profile" << endl;
                    break;
                case 5:
                    cout << "Player Walk" << endl;
                    break;
                case 6:
                    cout << "Player Sleep" << endl;
                    break;
                default:
                    break;
            }
            cout << endl;
            cout << "Success = " << bSuccess << endl;
        }
        
        
    } while (strcmp (command,quit) != 0);  

    cout << "Quitting client" << endl;  
    
    return 0;
}

bool SyncedMessage(TCHAR* command, vector<TCHAR*> args)
{
    
    bool success = false;
    TCHAR chReadBuf[PIPE_BUFFER_SIZE]; 

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
        PIPE_BUFFER_SIZE,  // size of read buffer 
        &cbRead,                // number of bytes read 
        PIPE_TIMEOUT
    );                 // waits for 20 seconds 
 
    if (success || GetLastError() == ERROR_MORE_DATA) 
    { 
        _tprintf( TEXT("\nServer: %s\n"), chReadBuf ); 
        
        // The pipe is closed; no more data can be read. 
    
        if (!success) 
        {
            printf("\nExtra data in message was lost\n"); 
        }
    } 
    return success;
}

bool ASyncedMessage(TCHAR* command, vector<TCHAR*> args)
{
    bool success = FALSE;

    HANDLE hThread = NULL;     // Thread Handle
    DWORD dwThreadId = 0;      // Sore thread ID

    struct CommandAndArgs* data = new CommandAndArgs;
    data->command = command;
    data->args = args;

    hThread = CreateThread(
        NULL,                           // No security attribute
        0,                              // Default stack size
        InstanceThread,                 // Thread Process
        (LPVOID) data,      // Thread Parameter
        0,                              // Not suspended
        &dwThreadId                     // Returns thread ID
    );

    return success;
}

DWORD WINAPI InstanceThread(LPVOID lpvParam)
{
    HANDLE hHeap = GetProcessHeap();
    struct CommandAndArgs* data = (struct CommandAndArgs*) lpvParam;

    if(data == NULL)
    {
        _tprintf("   Client Failure:");
        _tprintf("   Instance Thread got and unexpected NULL value in lpvParam");
        _tprintf("   Instance Thread exiting");
        return (DWORD)-1;
    }

    bool fSuccess = false;

    fSuccess = SyncedMessage(data->command, data->args);

    return 1;
}