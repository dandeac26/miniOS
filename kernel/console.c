#include "console.h"
#include "mem.h"


char screen_buffer[MAX_OFFSET];


int GetCommandNumber(const char* cmd, size_t size) 
{

    const char CommandList[][15] = { "cls", "edit", "time", "printmbr", "test_run", "test_run_all", "test_list"};
    int numCommands = sizeof(CommandList) / sizeof(CommandList[0]);

    for (int i = 0; i < numCommands; i++)
    {
        int j = 0;

        while (j < size && CommandList[i][j] != '\0' && cmd[j] == CommandList[i][j]) 
        {
            j++;
        }

        if (j == size && CommandList[i][j] == '\0') 
        {
            return i + 1;
        }
    }
    return 0;
}


void PrintTimeTillBoot() 
{
    int minutes = 0, seconds = 0;
    GetTimeTillBoot(&minutes, &seconds);
    LogSerialAndScreen("Time since boot: %d min:%ds\n", minutes, seconds);
}

#pragma optimize("", off)
void PrintMBR() 
{
    int drive = 0;
    if (DetectedATADevice(drive) == true)
    {
        BYTE buffer[SECTOR_SIZE] = { 0 }; // MBR is exactly one sector (512 bytes)

        ata_send_command(drive, ATA_CMD_READ_SECTORS, 0, buffer, 1);

        PutHexViewString(buffer, SECTOR_SIZE);
    }
}
#pragma optimize("", on)


void printInvalidCMD() 
{
    LogSerialAndScreen("Invalid Command!\n");
}


void test_run(int argLen, char* arg)
{
    if (argLen != 0) 
    {
        LogSerialAndScreen("%s\n", arg);
    }

    QWORD newFrame;
    DWORD frameCount = 1; // Number of frames to allocate

    if (1 == 1) 
    {
        LogSerialAndScreen("Testing true: %d\n", 1);
    }

    BOOLEAN result = frame_alloc(&newFrame, frameCount);

    LogSerialAndScreen("frame_alloc result: %d\n", result);
    if (result)
    {
        LogSerialAndScreen("Allocated frame at address: %X\n", newFrame);
    }
    else 
    {
        LogSerialAndScreen("Failed to allocate frame.\n");
    }

    QWORD newFrame1;
    DWORD frameCount1 = 2;


    BOOLEAN result1 = frame_alloc(&newFrame1, frameCount1);

    LogSerialAndScreen("frame_alloc result: %d\n", result1);
    if (result1)
    {
        LogSerialAndScreen("Allocated frame1 at address: %X\n", newFrame1);
    }
    else
    {
        LogSerialAndScreen("Failed to allocate frame.\n");
    }

    BOOLEAN result_realloc = frame_alloc(&newFrame, frameCount);
    if (result_realloc)
    {
        LogSerialAndScreen("Reallocated frame at address: %X\n", newFrame);
    }
    else
    {
        LogSerialAndScreen("Failed to allocate frame.\n");
    }


    frame_free(newFrame1, frameCount1);

    BOOLEAN result_alloc_after_free = frame_alloc(&newFrame1, frameCount1);
    
    if (result_alloc_after_free)
    {
        LogSerialAndScreen("Allocated recently freed frame1 at address: %X\n", newFrame1);
    }
    else
    {
        LogSerialAndScreen("Failed to allocate frame.\n");
    }

    LogSerialAndScreen("Are frames free before frame_free? : %d\n", are_frames_free(newFrame1, frameCount1));

    frame_free(newFrame1, frameCount1);

    LogSerialAndScreen("Are frames free after frame_free? : %d\n", are_frames_free(newFrame1, frameCount1));

    LogSerialAndScreen("test_run was ran!\n");
}


void test_run_all()
{
    LogSerialAndScreen("test_run_all was ran!\n");
}


void test_list()
{
    LogSerialAndScreen("test_list was ran!\n");
}


void RunCommand(int cmd, int argLen, char* arg)
{
    switch (cmd)
    {
        case 1: // cls
             ClearScreen();
            break;
        case 2: // edit
             EnterMode(EDIT_MODE);
            break;
        case 3:
            PrintTimeTillBoot();
            break;
        case 4:
            PrintMBR();
            break;
        case 5:
            test_run(argLen, arg);
            break;
        case 6:
            test_run_all();
            break;
        case 7:
            test_list();
            break;
        case 0:
            printInvalidCMD();
            break;
        default:
        
            break;
    }
}




int is_format_char(char c) 
{
    return c == '\t' || c == '\n' || c == '\r';
}

int is_value(char C) 
{
    return (C == KEY_SPACE || (C >= 'A' && C <= 'Z') || (C >= 'a' && C <= 'z') || (C >= '0' && C <= '9') || (C == '.' || C == ',' || C == ';' || C == '-' || C=='_' ||  C == '\''));
}


void ParseCommand(char* Buffer, size_t size) 
{
    int start = 0;

    // Skip leading whitespace or chars
    while (start < size && (is_format_char(Buffer[start]) || Buffer[start] == ' ')) 
    {
        start++;
    }

    size_t cmdLength = 0;
    char Command[MAX_COLUMNS];

    while (start < size && is_value(Buffer[start]) && Buffer[start] != ' ')
    {
        Command[cmdLength++] = Buffer[start];
        start++;
    }

    Command[cmdLength] = '\0';

    size_t argLength = 0;
    char Arg[MAX_COLUMNS];

    if (Buffer[start++] == ' ')
    {
        while (start < size && is_value(Buffer[start]) && Buffer[start] != ' ')
        {
            Arg[argLength++] = Buffer[start];
            start++;
        }
    } 

    Arg[argLength] = '\0';

    RunCommand(GetCommandNumber(Command, cmdLength), argLength, Arg);
}
