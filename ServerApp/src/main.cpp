#include <iostream>
#include <string>

#include "common/common.h"

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

    bConnectNamedPipe = ConnectNamedPipe(hCreateNamedPipe, NULL);
    if (bConnectNamedPipe)
    {
        cout << "Connection Failed & Error No - " << GetLastError() << endl;
        return -1;
    }
    cout << "Connected successfully = " << endl;


    return 0;
}

