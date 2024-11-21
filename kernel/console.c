#include "console.h"


char screen_buffer[MAX_OFFSET];


int GetCommandNumber(const char* cmd, size_t size) 
{

    const char CommandList[][10] = { "cls", "edit", "time", "printmbr"};
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

void printInvalidCMD() {
    /*char msg[] = "Not a valid command!";
    int len = 0;
    while (msg[len] != '\0') {
        len++;
    }
    PutString(msg, len);*/

    LogSerialAndScreen("Invalid Command!\n");
}

void RunCommand(int cmd)
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
    return (C == KEY_SPACE || (C >= 'A' && C <= 'Z') || (C >= 'a' && C <= 'z') || (C >= '0' && C <= '9') || (C == '.' || C == ',' || C == ';' || C == '-' || C == '\''));
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

    RunCommand(GetCommandNumber(Command, cmdLength));
}
