#include <iostream>
#include <string>
#include <tchar.h>

#include "common/common.h"
#include "common/userProfile.h"

// Client pipe based of Microsoft's
// https://docs.microsoft.com/en-us/windows/desktop/ipc/named-pipe-client
// https://docs.microsoft.com/en-us/windows/desktop/ipc/transactions-on-named-pipes

// CommandAndArgs struct created for threading with the created Sync and Async methods
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
    // Client reception and variables initialization
    printf("Hello, CMake Client!\n");
    LPTSTR lpszWrite = TEXT("Default message");
    char command[PIPE_BUFFER_SIZE];
    char quit[] = "Exit";
    CommonLib cObj;
    string availableCommands = cObj.GetCommandListAsString();
    int commandIndex;

    do {
        // Run this loop while the client doesn't input "Exit"
        // Get client's desired command
        cout << "Choose a command from: " << availableCommands << ", " << quit << endl;
        cin.getline(command, PIPE_BUFFER_SIZE);
        if (strcmp (command,quit) == 0)
            return 0;

        // Check if the command exists on CommonLib
        commandIndex = cObj.CommandExists(command);
        
        if (commandIndex >= 0)
        {
            cout << "Client Running: " ;
            switch(commandIndex){
                case 0:
                    {
                        // If the command is Async Hello, we simply run ASyncedMessage
                        cout << "Asynced Hello" << endl;
                        ASyncedMessage(cObj.StringToTCHAR(cObj.GetCommand(commandIndex)), {});
                    }
                    break;
                case 1:
                    {
                        // If the command is Sync Hello, we simply run SyncedMessage
                        cout << "Synced Hello" << endl;
                        SyncedMessage(cObj.StringToTCHAR(cObj.GetCommand(commandIndex)), {});
                    }
                    break;
                case 2:
                    {
                        // If the command is Send Name, we get the client's input and send as an arg
                        // to AsyncedMessage
                        cout << "Send Name" << endl;
                        cout << "What name do you want to send?" << endl;
                        char nameToSend[PIPE_BUFFER_SIZE];
                        cin.getline(nameToSend, PIPE_BUFFER_SIZE);
                        ASyncedMessage(cObj.StringToTCHAR(cObj.GetCommand(commandIndex)), {nameToSend});
                    }
                    break;
                case 3:
                    {
                        // Pretty similar to Send Name, but here we will run a function called GetIntFromuser
                        // To get the input from the client and make sure it's a valid int.
                        // Also, we will use a Synced Message just to show that both ways work.
                        cout << "Send Age" << endl;
                        cout << "How old are you?" << endl;
                        int age = GetIntFromUser();
                        char ageToSend[PIPE_BUFFER_SIZE];
                        sprintf_s(ageToSend, "%d", age);
                        SyncedMessage(cObj.StringToTCHAR(cObj.GetCommand(commandIndex)), {ageToSend});
                    }
                    break;
                case 4:
                    {
                        // We will send an ASync request to create the user profile
                        // passing all args required, after asking them from the user
                        cout << "Create User Profile" << endl;
                        
                        cout << "Please input the user's name" << endl;
                        char nameToSend[PIPE_BUFFER_SIZE];
                        cin.getline(nameToSend, PIPE_BUFFER_SIZE);

                        cout << "Please input the user's age" << endl;
                        int age = GetIntFromUser();
                        char ageToSend[PIPE_BUFFER_SIZE];
                        sprintf_s(ageToSend, "%d", age);

                        cout << "Please input the user's Street" << endl;
                        char streetToSend[PIPE_BUFFER_SIZE];
                        cin.getline(streetToSend, PIPE_BUFFER_SIZE);

                        cout << "Please input the user's City" << endl;
                        char cityToSend[PIPE_BUFFER_SIZE];
                        cin.getline(cityToSend, PIPE_BUFFER_SIZE);

                        cout << "Please input the user's Country" << endl;
                        char countryToSend[PIPE_BUFFER_SIZE];
                        cin.getline(countryToSend, PIPE_BUFFER_SIZE);

                        cout << "Please input the user's postal Code" << endl;
                        char postaToSend[PIPE_BUFFER_SIZE];
                        cin.getline(postaToSend, PIPE_BUFFER_SIZE);

                        ASyncedMessage(cObj.StringToTCHAR(cObj.GetCommand(commandIndex)), {nameToSend, ageToSend, streetToSend, cityToSend, countryToSend, postaToSend});
                        
                    }
                    break;
                case 5:
                    {
                        // We will send a ASync request to run a function on the desired user
                        // This function will return a value 
                        cout << "User Birthday" << endl;
                    }
                    break;
                case 6:
                    {
                        // We will send a Sync request to get a user profile, given an id
                        cout << "Find User Profile" << endl;
                    }
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
    // To create a synced message we need to lock the user while he waits for the answer
    // Making the whole program wait for the answer

    // Create vars
    bool success = false;
    TCHAR chReadBuf[PIPE_BUFFER_SIZE];  // Reply buffer
    string treatedArgs = "";
    LPTSTR lpszMessage = TEXT("");      // Message that we will send
    CommonLib cObj;
    DWORD cbRead;                       // Number of bytes that were read

    // For each argument the user sent we will concatenate it with an '@' as a separator.
    for(size_t i = 0; i < args.size(); i++)
    {
        treatedArgs += "@" + (string)args[i];
    }
        
    // If there were commands to treat we will add them to the request.
    string newCommand = command;
    if (treatedArgs.length() > 0)
        newCommand += treatedArgs;

    cout << endl << "sending: " << newCommand << endl;
    lpszMessage = cObj.StringToTCHAR(newCommand);
    
    // We could call Create File, WriteFile, ReadFile, CloseHandle, etc.
    // But it is easir, more contained and resembles more a restfull service
    // to use just CallNamedPipe. 
    success = CallNamedPipe( 
        PIPE_NAME,                              // pipe name 
        lpszMessage,                            // message to server 
        (lstrlen(lpszMessage)+1)*sizeof(TCHAR), // message length 
        chReadBuf,                              // buffer to receive reply 
        PIPE_BUFFER_SIZE,                       // size of read buffer 
        &cbRead,                                // number of bytes read 
        PIPE_TIMEOUT                            // waits for 20 seconds 
    );                 
 
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
        _tprintf( TEXT("\nERROR: Problem with pipe, code %d\n"), GetLastError() ); 
    }
}

void ASyncedMessage(TCHAR* command, vector<TCHAR*> args)
{
    // An Asynchronous task is a task that won't make the program wait for it.
    // We decided to create a thread every time the user need a Async message.
    // This thread will call SyncedMessage and operate as if it was Synced.
    // But, since it's running on it's own thread, it won't affect the Client's.
    // We could use an overlaped I/O server and a threaded client for this, but
    // I believe this approach is simpler, due to not needing to handle the server's
    // data on the pipe and dealing with everything as if it was synced.
    HANDLE hThread = NULL;     // Thread Handle
    DWORD dwThreadId = 0;      // Store thread ID

    // Command and Args as a struct to pass to the thread
    struct CommandAndArgs* data = new CommandAndArgs;
    data->command = command;
    data->args = args;

    // Thread creation.
    hThread = CreateThread(
        NULL,                           // No security attribute
        0,                              // Default stack size
        InstanceThread,                 // Thread Process
        (LPVOID) data,                  // Thread Parameter
        0,                              // Not suspended
        &dwThreadId                     // Returns thread ID
    );

}

DWORD WINAPI InstanceThread(LPVOID lpvParam)
{
    struct CommandAndArgs* data = (struct CommandAndArgs*) lpvParam;
    // Get the data sent as a CommandAndArgs Struct

    if(data == NULL)
    {
        _tprintf("   Client Failure:");
        _tprintf("   Instance Thread got and unexpected NULL value in lpvParam");
        _tprintf("   Instance Thread exiting");
        return (DWORD)-1;
    }

    // Send SyncedMessage
    SyncedMessage(data->command, data->args);

    return 1;
}

int GetIntFromUser()
{
    // Simple loop to get a valid integer from the user, rejecting other data.
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