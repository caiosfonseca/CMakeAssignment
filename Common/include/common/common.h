#include <string>
#include <windows.h>

#define INSTANCES 4 
#define PIPE_TIMEOUT 5000
#define BUFFER_SIZE 4096
#define PIPE_NAME "\\\\.\\pipe\\Assignment"
#define OPEN_MODE PIPE_ACCESS_DUPLEX //CHANGE TO PIPE_ACCESS_DUPLEX | FLIE_FLAG_OVERLAPPED
#define PIPE_MODE PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT //CHANGE TO PIPE_NOWAiT later

class common
{
    public:
        void ReadConfigFile();
};