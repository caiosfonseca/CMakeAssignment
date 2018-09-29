#include <string>
#include <windows.h>
#include <vector>

#define INSTANCES PIPE_UNLIMITED_INSTANCES // Amount of pipe isntances
#define PIPE_TIMEOUT 5000 // Pipe timeout to 5 seconds
#define BUFFER_SIZE 4096 // 
#define PIPE_NAME "\\\\.\\pipe\\Assignment"
#define OPEN_MODE PIPE_ACCESS_DUPLEX //CHANGE TO PIPE_ACCESS_DUPLEX | FLIE_FLAG_OVERLAPPED
#define PIPE_MODE PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT //CHANGE TO PIPE_NOWAiT later
#define READ_WRITE_MODE GENERIC_READ | GENERIC_WRITE 
#define PIPE_BUFFER_SIZE BUFFER_SIZE*sizeof(TCHAR)

using namespace std;

class CommonLib
{
    private:
        vector<string> commandList;

    public:
        CommonLib();

        void ReadConfigFile();

        int CommandExists(const string &value);
        string GetCommandListAsString();
        string GetCommand(const int &val);

        TCHAR* StringToTCHAR(const string &value);
};