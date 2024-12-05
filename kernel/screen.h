#ifndef _SCREEN_H_
#define _SCREEN_H_

#include "main.h"
#include "scancode.h"
#include "console.h"
#include "string.h"

#define TOTAL_OFFSET   16000   //80000 = 1000 lines * 80 chars
#define TOTAL_MAX_LINES 200   //1000
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


typedef struct _SCREEN_STATE
{
    int col[TOTAL_MAX_LINES]; // for each line say where the cursor is on the columns
    int row; // current row of cursor
    int line_size[TOTAL_MAX_LINES]; // each line 's size
    int new_line[TOTAL_MAX_LINES];  // used to make difference between empty lines and new lines
    char Buffer[TOTAL_OFFSET];
    int view_offset; // for scrolling (offsets current screen buffer)
}SCREEN_STATE;

static SCREEN_STATE NormalScreen;
static SCREEN_STATE EditScreen;
static SCREEN_STATE CurrentScreen;

void SetColor(BYTE Color);
void ClearScreen();
void PutChar(KEYCODE C, int is_ext);

void PutHexViewString(char* buffer, size_t size);
void PutString(char* buffer, size_t size);

void PutCharExt(KEYCODE C);
void PutCharStd(KEYCODE C);

void InitScreen();
#endif // _SCREEN_H_