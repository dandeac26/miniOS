
#include "console.h"
#include "screen.h"

char Cmd1[] = "cls";

void RunCommand(const char *cmd, size_t size)
{
    /*int i;
    for (i = 0; (i < size) && (i < MAX_OFFSET); i++)
    {
        gVideo[i].color = 10;
        gVideo[i].c = Buffer[i];
    }
    CursorPosition(i);*/
    
    //LogSerialAndScreen("aaaaaaaaaaaaaaa %c", cmd[0]);
            
    /*int isCmd1 = true;
    for (size_t i = 0; i < size; i++) {
        if (Buffer[i] != Cmd1[i]) {
            isCmd1 = false;
            break;
        }
    }
    if (isCmd1 == true) {
        ScreenDisplay("CLS WAS RAN!", 10);
    }*/
    
    
}


int is_format_char(char c) {
    return c == '\t' || c == '\n' || c == '\r';
}




void ParseCommand(char Buffer[], size_t size) {
    int start = 0;

    // Skip leading whitespace or chars
    while (start < size && (is_format_char(Buffer[start]) || Buffer[start] == ' ')) {
        start++;
    }

    
    size_t cmdLength = 0;
    char Command[MAX_OFFSET];
    while (start < size && !is_format_char(Buffer[start]) && Buffer[start] != ' ' && cmdLength < size - 1) {
        Command[cmdLength] = Buffer[start];
        cmdLength++;
        start++;
    }

    Command[cmdLength] = '\0';   
    
    //ScreenDisplay(Command, 10);
    RunCommand(Command, cmdLength);
}


void CClearScreen(void* VideoMemoryBuffer,   // if NULL don't store the previous content
                  DWORD   BufferSize,
                  int* CursorPosition // if NULL don't save cursor position

) {

}


void RestoreScreen(

    void* VideoMemoryBuffer,

    DWORD   BufferSize,

    int     CursorPosition
) {

}