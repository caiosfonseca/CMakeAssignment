#include <iostream>
#include <string>
#include <tchar.h>
#include <strsafe.h>

#include "common/common.h"

VOID GetAnswerToRequest( LPTSTR pchRequest, LPTSTR pchReply, LPDWORD pchBytes );

int main(int argc, char* argv[])
{
    //Main Vars

    HANDLE hCreateNamedPipe = INVALID_HANDLE_VALUE; //NamdePipe Handle
    BOOL bConnectNamedPipe = FALSE; //Connection bool
    HANDLE hThread = NULL; //NamdePipe Handle



    printf("Hello, CMake Server!\n");

        
    DWORD szPipeBuffer = BUFFER_SIZE*sizeof(TCHAR);//Size of pipe buffer
    // Creating Named Pipe
    hCreateNamedPipe = CreateNamedPipe(
                        PIPE_NAME,
                        OPEN_MODE,
                        PIPE_MODE,
                        INSTANCES,
                        szPipeBuffer,
                        szPipeBuffer,
                        PIPE_TIMEOUT,
                        NULL
    );

    if(hCreateNamedPipe == INVALID_HANDLE_VALUE)
    {
        cout << "Named Pipe creation failed, error = " << GetLastError() << endl;
        return -1;
    }

    cout << "Named Pipe created with success " << endl;

    // Connecting to Named Pipe

    // Wait for the client to connect; if it succeeds, 
    // the function returns a nonzero value. If the function
    // returns zero, GetLastError returns ERROR_PIPE_CONNECTED. 
    bConnectNamedPipe = ConnectNamedPipe(hCreateNamedPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
    if (bConnectNamedPipe)
    {
        cout << "Connected successfully = " << endl;
    }
    else{
        cout << "Connection Failed & Error No - " << GetLastError() << endl;
        CloseHandle(hCreateNamedPipe);
        return -1;
    }

    bool fSuccess = false;
    TCHAR pchRequest [BUFFER_SIZE*sizeof(TCHAR)];
    TCHAR pchReply [BUFFER_SIZE*sizeof(TCHAR)];
    DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;
    while (1) 
    { 
    // Read client requests from the pipe. This simplistic code only allows messages
    // up to BUFSIZE characters in length.
        cout << "Waiting for client message" << endl;
        fSuccess = ReadFile( 
            hCreateNamedPipe,        // handle to pipe 
            pchRequest,    // buffer to receive data 
            BUFFER_SIZE*sizeof(TCHAR), // size of buffer 
            &cbBytesRead, // number of bytes read 
            NULL);        // not overlapped I/O 

        if (!fSuccess || cbBytesRead == 0)
        {   
            if (GetLastError() == ERROR_BROKEN_PIPE)
            {
                _tprintf(TEXT("InstanceThread: client disconnected. Error = %d\n"), GetLastError()); 
            }
            else
            {
                _tprintf(TEXT("InstanceThread ReadFile failed, GLE=%d.\n"), GetLastError()); 
            }
            break;
        }

    // Process the incoming message.
        cout << "Processing incoming message" << endl;
        GetAnswerToRequest(pchRequest, pchReply, &cbReplyBytes); 
    
    // Write the reply to the pipe. 
        cout << "Writing response for client" << endl;
        fSuccess = WriteFile( 
            hCreateNamedPipe,        // handle to pipe 
            pchReply,     // buffer to write from 
            cbReplyBytes, // number of bytes to write 
            &cbWritten,   // number of bytes written 
            NULL);        // not overlapped I/O 

        if (!fSuccess || cbReplyBytes != cbWritten)
        {   
            _tprintf(TEXT("InstanceThread WriteFile failed, GLE=%d.\n"), GetLastError()); 
            break;
        }
    }


    return 0;
}


VOID GetAnswerToRequest( LPTSTR pchRequest, 
                         LPTSTR pchReply, 
                         LPDWORD pchBytes )
// This routine is a simple function to print the client request to the console
// and populate the reply buffer with a default data string. This is where you
// would put the actual client request processing code that runs in the context
// of an instance thread. Keep in mind the main thread will continue to wait for
// and receive other client connections while the instance thread is working.
{
    _tprintf( TEXT("Client Request String:\"%s\"\n"), pchRequest );
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
        printf("StringCchCopy failed, no outgoing message.\n");
        return;
    }
    *pchBytes = (lstrlen(pchReply)+1)*sizeof(TCHAR);
}
