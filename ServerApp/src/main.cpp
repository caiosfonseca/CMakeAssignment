#include <iostream>
#include <string>

#include "common/common.h"

using namespace std;

int main(int argc, char* argv[])
{
    printf("Hello, CMake Server!\n");
  
    // pipeName = TEXT("\\\\.\\pipe\\mynamedpipe"); 
    // Creating Named Pipe
    HANDLE hCreateNamedPipe;
    DWORD szPipeBuffer = BUFFER_SIZE*sizeof(TCHAR);

    hCreateNamedPipe = CreateNamedPipeA(
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

    

    return 0;
}

