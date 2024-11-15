#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "main.h"
#include "screen.h"
#include "interrupts.h"

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

#define BACKSPACE_KEY 8


typedef enum _CONSOLE_MODE {
    EDIT_MODE = 0,
    NORMAL_MODE = 1
}CONSOLE_MODE;

static CONSOLE_MODE ConsoleMode = NORMAL_MODE;
static int line_size = 0;
static int text_color = 10;


void CClearScreen(char* VideoMemoryBuffer, DWORD BufferSize, int* CursorPosition);


void RestoreScreen(

    char* VideoMemoryBuffer,

    DWORD   BufferSize,

    int     CursorPosition

);


void ParseCommand(char* Buffer, size_t size);
int is_format_char(char c);

#endif _CONSOLE_H_