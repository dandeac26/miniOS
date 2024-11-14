#ifndef _SCREEN_H_
#define _SCREEN_H_

#include "main.h"

#define MAX_LINES       25
#define MAX_COLUMNS     80
#define MAX_OFFSET      2000 //25 lines * 80 chars
#define ENTER_KEY 10
#define ENTER_KEY2 13

#pragma pack(push)
#pragma pack(1)
typedef struct _SCREEN
{
    char c;
    BYTE  color;
}SCREEN, *PSCREEN;
#pragma pack(pop)

static int current_line_offset = 0;

void HelloBoot();

void SetColor(BYTE Color);
void ClearScreen();
void PutChar(char C);
void PutString(char* String, int Pos);
void PutStringLine(char* String, int Line);


#endif // _SCREEN_H_