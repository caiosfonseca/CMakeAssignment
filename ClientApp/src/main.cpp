#include <iostream>
#include <string>
#include <tchar.h>

#include "common/common.h"
#include "common/userProfile.h"

// Client pipe based of Microsoft's
// https://docs.microsoft.com/en-us/windows/desktop/ipc/named-pipe-client
// https://docs.microsoft.com/en-us/windows/desktop/ipc/transactions-on-named-pipes

struct CommandAndArgs
{
    TCHAR* command;
    vector<TCHAR*> args;
};

void SyncedMessage(TCHAR* command, vector<TCHAR*> args);
void ASyncedMessage(TCHAR* command, vector<TCHAR*> args);
DWORD WINAPI InstanceThread(LPVOID);
int GetIntFromUser();

int _tmain(int argc, TCHAR *argv[])
{
    printf("Hello, CMake Client!\n");
    LPTSTR lpszWrite = TEXT("Default message");
    char command[PIPE_BUFFER_SIZE];
    char quit[] = "Exit";

    Address testAddress = Address("1316 Carling Ave", "Ottawa", "Canada", "K1Z7L0");
    cout << testAddress << endl;
    UserProfile testUser = UserProfile("Caio", 26, testAddress, 0);
    cout << testUser << endl;
    testUser.BirthDay();
    cout << testUser << endl;

    CommonLib cObj;
    string availableCommands = cObj.GetCommandListAsString();
    int commandIndex;

    do {
        //Get client's desired command
        cout << "Choose a command from: " << availableCommands << ", " << quit << endl;
        cin.getline(command, PIPE_BUFFER_SIZE);
        if (strcmp (command,quit) == 0)
            return 0;

        commandIndex = cObj.CommandExists(command);
        
        if (commandIndex >= 0)
        {
            cout << "Client Running: " ;
            switch(commandIndex){
                case 0:
                    {
                        cout << "Asynced Hello" << endl;
                        ASyncedMessage(cObj.StringToTCHAR(cObj.GetCommand(commandIndex)), {});
                    }
                    break;
                case 1:
                    cout << "Synced Hello" << endl;
                    SyncedMessage(cObj.StringToTCHAR(cObj.GetCommand(commandIndex)), {});
                    break;
                case 2:
                    {
                        cout << "Send Name" << endl;
                        cout << "What name do you want to send?" << endl;
                        char nameToSend[PIPE_BUFFER_SIZE];
                        cin.getline(nameToSend, PIPE_BUFFER_SIZE);
                        cout << "Sending " << nameToSend << endl;
                        ASyncedMessage(cObj.StringToTCHAR(cObj.GetCommand(commandIndex)), {nameToSend});
                    }
                    break;
                case 3:
                    {
                        cout << "Send Age" << endl;
                        cout << "How old are you?" << endl;
                        int age = GetIntFromUser();
                        char ageToSend[PIPE_BUFFER_SIZE];
                        sprintf_s(ageToSend, "%d", age);
                        cout << "Sending " << ageToSend << endl;
                        SyncedMessage(cObj.StringToTCHAR(cObj.GetCommand(commandIndex)), {ageToSend});
                    }
                    break;
                case 4:
                    cout << "Create User Profile" << endl;
                    break;
                case 5:
                    cout << "Update Address" << endl;
                    break;
                case 6:
                    cout << "Find User Profile" << endl;
                    break;
                default:
                    break;
            }
            cout << endl;
        }
        
        
    } while (strcmp (command,quit) != 0);  

    cout << "Quitting client" << endl;  
    
    return 0;
}

void SyncedMessage(TCHAR* command, vector<TCHAR*> args)
{
    
    bool success = false;
    TCHAR chReadBuf[PIPE_BUFFER_SIZE]; 

    string treatedArgs = "";

    
    for(size_t i = 0; i < args.size(); i++)
    {
        treatedArgs += "@" + (string)args[i];
    }
        

    LPTSTR lpszMessage = TEXT("");
    CommonLib cObj;

    string test = command;
    if (treatedArgs.length() > 0)
        test += treatedArgs;
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
    else
    {
        _tprintf( TEXT("\nERROR: %d\n"), GetLastError() ); 
    }
}

void ASyncedMessage(TCHAR* command, vector<TCHAR*> args)
{

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

    SyncedMessage(data->command, data->args);

    return 1;
}

int GetIntFromUser()
{
    int age = -1;
    char answer[PIPE_BUFFER_SIZE];
    while(age < 0)
    {
        cin.getline(answer, PIPE_BUFFER_SIZE);
        try
        {
            age = stoi(answer);
        }
        catch(const invalid_argument& e)
        {
            cout << "Please, type a valid age!";
            (void)e;
        }
    }
    return age;
}