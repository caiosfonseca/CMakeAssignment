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
    TCHAR * pchReply = (TCHAR*)HeapAlloc(hHeap, 0, PIPE_BUFFER_SIZE);

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
    serverPrint((string ("CLIENT REQUEST String:\"%s\"\n"), pchRequest) );
    CommonLib cObj;
    string response = "default answer from server";

    if(pchRequest == cObj.GetCommand(1)){
        response = "Synchronized Hello!";
    }

    // Check the outgoing message to make sure it's not too long for the buffer.
    if (FAILED(StringCchCopy( pchReply, BUFFER_SIZE, response.c_str() )))
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