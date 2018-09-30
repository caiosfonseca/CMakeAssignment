#include <iostream>
#include <string>
#include <tchar.h>
#include <strsafe.h>

#include "common/common.h"

// Multithreaded pipe server based of Microsoft's
// https://docs.microsoft.com/en-us/windows/desktop/ipc/multithreaded-pipe-server

DWORD WINAPI InstanceThread(LPVOID);
VOID GetAnswerToRequest( LPTSTR pchRequest, LPTSTR pchReply, LPDWORD pchBytes );
VOID serverPrint(string Messages, bool isError = FALSE);

string lastSentName = "";
string lastSentAge = "";

int _tmain(int argc, char* argv[])
{
    //Defining vars
    HANDLE hCreateNamedPipe = INVALID_HANDLE_VALUE; // NamdePipe Handle
    BOOL bConnectNamedPipe = FALSE;                 // Connection bool
    HANDLE hThread = NULL;                          // Thread Handle
    DWORD dwThreadId = 0;                           // Sore thread ID

    serverPrint("Hello, CMake Server!");

    // The main loop runs an infinite for loop.
    // For each loop's iteraction, a NamedPipe instance is created and waits for a client connection
    // Once it connects, it creates a thread to handle communication and continues the loop,
    // Effectively waiting for the next connection.
    // This allows multiple clients and also for them to work simultaneously.

    for(;;)
    {
        serverPrint( (string("* Main thread awaiting client connection on ") + PIPE_NAME) );
        // Creating named pipe
        hCreateNamedPipe = CreateNamedPipe(
                            PIPE_NAME,
                            OPEN_MODE,
                            PIPE_MODE,
                            INSTANCES,
                            PIPE_BUFFER_SIZE,
                            PIPE_BUFFER_SIZE,
                            PIPE_TIMEOUT,
                            NULL
        );

        if (hCreateNamedPipe == INVALID_HANDLE_VALUE)
        {
            serverPrint( string("CreateNamedPipe failed, Error = ") +  to_string(GetLastError()), true );
            return -1;
        }
        serverPrint( string("Created Named Pipe with success") );

        bConnectNamedPipe = ConnectNamedPipe(hCreateNamedPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 
        // Accoding to documentation:  It is possible for a pipe client to connect successfully to the pipe instance in the interval between
        // calls to the CreateNamedPipe and ConnectNamedPipe functions. If this happens, ConnectNamedPipe returns zero, and GetLastError
        // returns ERROR_PIPE_CONNECTED.
        
        if(bConnectNamedPipe)
        {
            serverPrint("Client connected, creating a processing thread");

            // Create a thread for this client
            hThread = CreateThread(
                NULL,                           // No security attribute
                0,                              // Default stack size
                InstanceThread,                 // Thread Process
                (LPVOID) hCreateNamedPipe,      // Thread Parameter
                0,                              // Not suspended
                &dwThreadId                     // Returns thread ID
            );

            if (hThread == NULL)
            {
                serverPrint( string("Create Thread Failed, Error = ") + to_string(GetLastError()), true );
                return -1;
            }
            else 
                // Close thread if there was a problem with creation
                CloseHandle(hThread);
        }
        else
        {
            // Closes the pipe if client couldn't connect
            serverPrint("Client couldn't connect", true);
            CloseHandle(hCreateNamedPipe);
        }
    }
    return 0;
}

DWORD WINAPI InstanceThread(LPVOID lpvParam)
{
    HANDLE hHeap = GetProcessHeap();
    TCHAR* pchRequest = (TCHAR*)HeapAlloc(hHeap, 0, PIPE_BUFFER_SIZE);
    TCHAR* pchReply = (TCHAR*)HeapAlloc(hHeap, 0, PIPE_BUFFER_SIZE);

    DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;
    BOOL fSuccess = FALSE;
    HANDLE hPipe = NULL;

    // Pipe handle == NULL
    if(lpvParam == NULL)
    {
        serverPrint("   Pipe Server Failure:", true);
        serverPrint("   Instance Thread got and unexpected NULL value in lpvParam", true);
        serverPrint("   Instance Thread exiting", true);
        // Free used memory allocations
        if ( pchReply != NULL ) HeapFree(hHeap, 0, pchReply);
        if ( pchRequest != NULL ) HeapFree(hHeap, 0, pchRequest);
        return (DWORD)-1;
    }

    // Request == null
    if(pchRequest == NULL)
    {
        serverPrint("   Pipe Server Failure:", true);
        serverPrint("   Instance Thread got and unexpected NULL heap allocation on pchRequest", true);
        serverPrint("   Instance Thread exiting", true);
        // Free used memory allocations
        if ( pchReply != NULL ) HeapFree(hHeap, 0, pchReply);
        return (DWORD)-1;
    }

    if(pchReply == NULL)
    {
        serverPrint("   Pipe Server Failure:", true);
        serverPrint("   Instance Thread got and unexpected NULL heap allocation on pchReply", true);
        serverPrint("   Instance Thread exiting", true);
        // Free used memory allocations
        if ( pchRequest != NULL ) HeapFree(hHeap, 0, pchRequest);
        return (DWORD)-1;
    }

    serverPrint("Instance thread created, receiving and processing messages.");

    hPipe = (HANDLE) lpvParam;

    while(1)
    {
        //Read client's requests
        fSuccess = ReadFile(
            hPipe,              // Handle to Pipe
            pchRequest,         // Buffer to receive data
            PIPE_BUFFER_SIZE,   // Size of buffer
            &cbBytesRead,       // Number of bytes read
            NULL                // Not overlapped I/O
        );

        if (!fSuccess || cbBytesRead == 0)
        {
            if(GetLastError() == ERROR_BROKEN_PIPE)
            {
                serverPrint( string("Instance Thread Client disconnected, Error = ") + to_string(GetLastError()), true );
            }
            else
            {
                serverPrint( string("Instance Thread Read File failed, Error = ") + to_string(GetLastError()), true );
            }
            break;
        }

        // Get the respective answer to the client's request
        GetAnswerToRequest(pchRequest, pchReply, &cbReplyBytes);

        fSuccess = WriteFile(
            hPipe,          // Pipe Handle
            pchReply,       // Buffer to write
            cbReplyBytes,   // Number of bytes to write
            &cbWritten,     // Number of bytes written
            NULL
        );

        if(!fSuccess || cbReplyBytes != cbWritten)
        {
            serverPrint( string("Instance Thread Write File failed, Error = ") + to_string(GetLastError()), true );
            break;
        }
    }

    // Flushing pipe buffers to allow client reading before disconnection.
    // Then, disconnect and close this pipe instance.

    FlushFileBuffers(hPipe);
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);

    HeapFree(hHeap, 0, pchRequest);
    HeapFree(hHeap, 0, pchReply);

    serverPrint("Instance Thread exitting.");
    return 1;
}


VOID GetAnswerToRequest( LPTSTR pchRequest, 
                         LPTSTR pchReply, 
                         LPDWORD pchBytes )
// Here we will deal with the client's request, and create the appropriate answer
// This will block the pipe thread, but not the main pipe thread, meaning that both
// the client and server can continue to work together while this function is running.
{
    serverPrint(string ("CLIENT REQUEST String: ") + pchRequest );
    // Create default server response.
    string response = "Command not recognized";

    // The user sent us a string of parameters separated by a @,  we will only distinct
    // which parameter is which and where to use them once we know which request the user sent

    // Creating and Initializing vars
    size_t firstAt = 0;
    string request = pchRequest;
    string args = "";
    size_t nOfArguments = 0;
    vector<string> treatedArgs;

    firstAt = request.find("@");
    
    if (firstAt > request.length())
    {
        // If we didn't find any @ we will simply set firstAt to the request length.
        // Later we will get the request using this var.
        firstAt = request.length();
    }
    else
    {
        // We know that all args will come after the first @ and the number of arguments is
        // equal to the number of @ sent in the request
        args = request.substr(firstAt+1);
        nOfArguments = count(request.begin(), request.end(), '@');

        // Variables creation and instancing
        size_t lastFoundAt = 0;
        size_t newFoundAt = 0;
        
        // Now, for each @ we find on the arguments list, we will add it to the vactor of 
        // arguments we created earlier.
        for(size_t i = 0; i < nOfArguments; i++)
        {
            newFoundAt = args.substr(lastFoundAt).find("@");
            if(newFoundAt > args.length())
            {
                newFoundAt = args.length();
            }
            treatedArgs.push_back(args.substr(lastFoundAt, newFoundAt));
            lastFoundAt = newFoundAt;
        }
    }

    request = request.substr(0, firstAt);
    serverPrint( "Treated request = " + request );

    // Server printing arguments just for the sake of logging
    if(treatedArgs.size() > 0)
    {
        for(size_t i = 0; i < treatedArgs.size(); i++)
        {
            serverPrint( "Arg["+ to_string(i) +"] = " + treatedArgs[i] );
        }        
    }
    else
    {
        serverPrint("No arguments sent");
    }
    
    // Now, we will deal with the client's request and respond appropriately
    CommonLib cObj;
    if(request == cObj.GetCommand(0))
    {
        response = "Asynchronized Hello brother!";
        if(lastSentName != "")
            response = "A special Asynchronized Hello for you " + lastSentName;
    }
    else if(request == cObj.GetCommand(1))
    {
        response = "I salute you with a Synchronized Hello!";
        if(lastSentName != "")
            response = "A special handshake for you " + lastSentName;
    }
    else if(request == cObj.GetCommand(2))
    {
        lastSentName = treatedArgs[0];
        response = "Oh Hi there, "+lastSentName+", I hope you are doing well!";
    }
    else if(request == cObj.GetCommand(3))
    {
        lastSentAge = treatedArgs[0];
        response = "I see, so you are " + lastSentAge + " years old, huh?";
        if(lastSentName != "")
            response = "Hey " + lastSentName + " I promise I won't tell that you are " + lastSentAge +" years old";
    }

    serverPrint("Sending = " + response);
    // Check the outgoing message to make sure it's not too long for the buffer.
    if (FAILED(StringCchCopy( pchReply, PIPE_BUFFER_SIZE, response.c_str() )))
    {
        *pchBytes = 0;
        pchReply[0] = 0;
        serverPrint("StringCchCopy failed, no outgoing message.", true);
        return;
    }
    *pchBytes = (lstrlen(pchReply)+1)*sizeof(TCHAR);
}


VOID serverPrint(string Message, bool isError)
{
    // Pretty simple print function for logs and errors
    string Log = "Log";
    if (isError)
        Log = "ERROR";
        
    _tprintf("Server %s: %s\n", Log.c_str(), Message.c_str());
}