#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "main.h"
#include <stdlib.h>  // For malloc and free
#include <ctype.h> 

#define BRIGHT_WHITE_COLOR          0xF
#define YELLOW_COLOR                0xE
#define BRIGHT_MAGENTA_COLOR        0xD
#define BRIGHT_RED_COLOR            0xC
#define BRIGHT_CYAN_COLOR           0xB
#define BRIGHT_GREEN_COLOR          0xA        
#define BRIGHT_BLUE_COLOR           0x9
#define GRAY_COLOR                  0x8
#define WHITE_COLOR                 0x7
#define BROWN_COLOR                 0x6
#define MAGENTA_COLOR               0x5
#define RED_COLOR                   0x4
#define CYAN_COLOR                  0x3
#define GREEN_COLOR                 0x2
#define BLUE_COLOR                  0x1
#define BLACK_COLOR                 0x0

void CClearScreen(

    void* VideoMemoryBuffer,   // if NULL don't store the previous content

    DWORD   BufferSize,

    int* CursorPosition // if NULL don't save cursor position

    );


void ParseCommand(char Buffer[], size_t size);

void RestoreScreen(

    void* VideoMemoryBuffer,

    DWORD   BufferSize,

    int     CursorPosition

);


#endif _CONSOLE_H_