#include "console.h"

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

void PrintMBR() {
    ClearScreen();
}


void RunCommand(int cmd)
{
    switch (cmd)
    {
        case 1: // cls
             ClearScreen();
            break;
        case 2: // edit
             EnterMode(EDIT_MODE); /// For some reason vede functia fara sa o pun in header ??
            break;
        case 3:
            PrintTimeTillBoot();
            break;
        case 4:
            PrintMBR();
            break;
        case 0:
            LogSerialAndScreen("Not a valid command!\n");
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
