
#include "console.h"
#include "screen.h"

void RunCommand(char Buffer[], size_t size)
{
    /*int i;
    for (i = 0; (i < size) && (i < MAX_OFFSET); i++)
    {
        gVideo[i].color = 10;
        gVideo[i].c = Buffer[i];
    }
    CursorPosition(i);*/

    
}


void ParseCommand(char Buffer[], size_t size) {
    // Skip leading whitespace
    size_t start = 0;
    while (start < size && isspace(Buffer[start])) {
        start++;
    }

    // Find the end of the first command
    size_t end = start;
    while (end < size && !isspace(Buffer[end])) {
        end++;
    }

    // Calculate the length of the command and allocate memory
    size_t cmdLength = end - start;
    char* firstCmd = malloc(sizeof(char) * (cmdLength + 1));  // +1 for null terminator

    // Copy the command into the allocated memory
    for (size_t i = 0; i < cmdLength; i++) {
        firstCmd[i] = Buffer[start + i];
    }
    firstCmd[cmdLength] = '\0';  // Null-terminate the command

    RunCommand(firstCmd, strlen(firstCmd));
    // Print the command for testing (or use it as needed)
  /*  printf("Extracted Command: %s\n", firstCmd);*/

    // Free the allocated memory when done
    free(firstCmd);
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