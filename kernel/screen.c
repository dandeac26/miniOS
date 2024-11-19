#include "screen.h"

static PSCREEN gVideo = (PSCREEN)(0x000B8000);
static char CLIBuffer[82];


//static char CurrentScreen.Buffer[MAX_OFFSET];
//
//static char NORMAL_VideoBuffer[MAX_OFFSET];

#define SCREEN_OFFSET (CurrentScreen.row * MAX_COLUMNS + CurrentScreen.col[CurrentScreen.row])

void CursorMove(int row, int col)
{
    unsigned short location = (row * MAX_COLUMNS) + col;       /* Short is a 16bit type , the formula is used here*/

    //Cursor Low port
    __outbyte(0x3D4, 0x0F);                                    //Sending the cursor low byte to the VGA Controller
    __outbyte(0x3D5, (unsigned char)(location & 0xFF));

    //Cursor High port
    __outbyte(0x3D4, 0x0E);                                    //Sending the cursor high byte to the VGA Controller
    __outbyte(0x3D5, (unsigned char)((location >> 8) & 0xFF)); //Char is a 8bit type
}

void CursorPosition(int pos)
{
    int row, col;

    if (pos > MAX_OFFSET)
    {
        pos = pos % MAX_OFFSET;
    }

    row = pos / MAX_COLUMNS;
    col = pos % MAX_COLUMNS;

    CursorMove(row, col);
}

// THIS WILL NO LONGER WORK
//void HelloBoot()
//{
//    int i, len;
//	char boot[] = "Hello Boot! Greetings from C... lol";
//
//	len = 0;
//	while (boot[len] != 0)
//	{
//		len++;
//	}
//
//	for (i = 0; (i < len) && (i < MAX_OFFSET); i++)
//	{
//		gVideo[i].color = text_color;
//		gVideo[i].c = boot[i];
//	}
//    CursorPosition(i);
//}

void IntBufferInit(int* buf, size_t size, int value) {
    for (int i = 0; i < size; i++) {
        buf[i] = value;
    }
}

#pragma optimize("", off)
void CharBufferInit(char* buf, size_t size, char value) {
    for (int i = 0; i < size; i++) {
        buf[i] = value;
    }
}
#pragma optimize("", off)

void InitScreen()
{
    ConsoleMode = NORMAL_MODE;

    IntBufferInit(CurrentScreen.col, MAX_LINES, 0);
    CurrentScreen.row = 0;
    IntBufferInit(CurrentScreen.line_size, MAX_LINES, 0);
    IntBufferInit(CurrentScreen.new_line, MAX_LINES, false);
    CharBufferInit(CurrentScreen.Buffer, MAX_OFFSET, ' ');

    IntBufferInit(NormalScreen.col, MAX_LINES, 0);
    NormalScreen.row = 0;
    IntBufferInit(NormalScreen.line_size, MAX_LINES, 0);
    IntBufferInit(NormalScreen.new_line, MAX_LINES, false);
    CharBufferInit(NormalScreen.Buffer, MAX_OFFSET, ' ');


    IntBufferInit(EditScreen.col, MAX_LINES, 0);
    EditScreen.row = 0;
    IntBufferInit(EditScreen.line_size, MAX_LINES, 0);
    IntBufferInit(EditScreen.new_line, MAX_LINES, false);
    CharBufferInit(EditScreen.Buffer, MAX_OFFSET, ' ');

    
}

#pragma optimize("", off)
void ClearScreen()
{
    int i;

    for (i = 0; i < MAX_OFFSET; i++)
    {
        gVideo[i].color = text_color;
        gVideo[i].c = ' ';
        CurrentScreen.Buffer[i] = ' ';
    }

    IntBufferInit(CurrentScreen.col, MAX_LINES, 0);
    CurrentScreen.row = 0;
    IntBufferInit(CurrentScreen.line_size, MAX_LINES, 0);
    IntBufferInit(CurrentScreen.new_line, MAX_LINES, false);

    //InitScreen();
 
    CursorPosition(SCREEN_OFFSET);
   
    //CursorMove(0, 0);
}
#pragma optimize("", on)
//void ClearScreenMode(CONSOLE_MODE mode)
//{
//    if (mode == NORMAL_MODE) {
//        int i;
//
//        for (i = 0; i < MAX_OFFSET; i++)
//        {
//            gVideo[i].color = text_color;
//            gVideo[i].c = ' ';
//            CurrentScreen.Buffer[i] = ' ';
//        }
//        for(int i = 0 ; i<MAX_LINES; i++)
//            CurrentScreen.col[i] = 0;
//        CurrentScreen.row = 0;
//
//        for (int i = 0; i < MAX_LINES; i++)
//            new_line[i] = false;
//
//        CursorPosition(SCREEN_OFFSET);
//        CursorMove(0, 0);
//    }
//    else {
//        int i;
//
//        for (i = 0; i < MAX_OFFSET; i++)
//        {
//            gVideo[i].color = BLUE_COLOR;
//            gVideo[i].c = ' ';
//            CurrentScreen.Buffer[i] = ' ';
//        }
//        gVideo[i].color = text_color;
//        for (int i = 0; i < MAX_LINES; i++)
//            CurrentScreen.col[i] = 0;
//        CurrentScreen.row = 0;
//
//        for (int i = 0; i < MAX_LINES; i++)
//            new_line[i] = 0;
//        CursorPosition(SCREEN_OFFSET);
//        CursorMove(0, 0);
//    }
//}

void SaveNormalScreen() 
{
    NormalScreen.row = CurrentScreen.row;
    //NormalScreen.col[CurrentScreen.row] = CurrentScreen.col[CurrentScreen.row];

    for (int i = 0; i < MAX_OFFSET; i++)
    {
        NormalScreen.Buffer[i] = CurrentScreen.Buffer[i];

        if (i < MAX_LINES)
        {
            NormalScreen.col[i] = CurrentScreen.col[i];
            NormalScreen.line_size[i] = CurrentScreen.line_size[i];
            NormalScreen.new_line[i] = CurrentScreen.new_line[i];
        }
    }
}

void SaveEditScreen() 
{
    EditScreen.row = CurrentScreen.row;
    //EditScreen.col[CurrentScreen.row] = CurrentScreen.col[CurrentScreen.row];

    for (int i = 0; i < MAX_OFFSET; i++)
    {
        EditScreen.Buffer[i] = CurrentScreen.Buffer[i];

        if (i < MAX_LINES)
        {
            EditScreen.col[i] = CurrentScreen.col[i];
            EditScreen.line_size[i] = CurrentScreen.line_size[i];
            EditScreen.new_line[i] = CurrentScreen.new_line[i];
        }
    }
    EditScreen.line_size[CurrentScreen.row] = CurrentScreen.line_size[CurrentScreen.row];
    EditScreen.col[CurrentScreen.row] = CurrentScreen.col[CurrentScreen.row];
    //EditScreen.new_line[CurrentScreen.row] = true;
}

#pragma optimize("", off)
void SaveScreenState(SCREEN_STATE state)
{
    state.row = CurrentScreen.row;
    state.col[CurrentScreen.row] = EditScreen.col[CurrentScreen.row];

    for (int i = 0; i < MAX_OFFSET; i++)
    {
        state.Buffer[i] = CurrentScreen.Buffer[i];

        if (i < MAX_LINES)
        {
            state.line_size[i] = CurrentScreen.line_size[i];
            state.new_line[i] = CurrentScreen.new_line[i];
        }
    }
}
#pragma optimize("", on)

#pragma optimize("", off)
void RestoreScreenState(SCREEN_STATE* state)
{
    for (int i = 0; i < MAX_OFFSET; i++)
    {
        gVideo[i].c = ' ';
        CurrentScreen.Buffer[i] = ' ';
    }

    if (ConsoleMode == EDIT_MODE) 
    {
        text_color = CYAN_COLOR;
    }
    else
    {
        text_color = 10;
    }

    CurrentScreen.row = state->row;

    for (int i = 0; i < MAX_OFFSET; i++)
    {
        gVideo[i].color = text_color;
        gVideo[i].c = state->Buffer[i];
        CurrentScreen.Buffer[i] = state->Buffer[i];

        if (i < MAX_LINES)
        {
            CurrentScreen.col[i] = state->col[i];
            CurrentScreen.line_size[i] = state->line_size[i];
            CurrentScreen.new_line[i] = state->new_line[i];
        }
    }

    if (ConsoleMode == EDIT_MODE) {
        if (CurrentScreen.col[CurrentScreen.row] != 0)
        {
            CurrentScreen.new_line[++CurrentScreen.row] = true;
        }
    }

    CursorPosition(SCREEN_OFFSET);
}
#pragma optimize("", on)

#pragma optimize("", off)
void RestoreEditScreen()
{
    for (int i = 0; i < MAX_OFFSET; i++)
    {
        gVideo[i].c = ' ';
        CurrentScreen.Buffer[i] = ' ';
    } 

    text_color = CYAN_COLOR;

    CurrentScreen.row = EditScreen.row;

    for (int i = 0; i < MAX_OFFSET; i++)
    {
        gVideo[i].color = text_color;
        gVideo[i].c = EditScreen.Buffer[i];
        CurrentScreen.Buffer[i] = EditScreen.Buffer[i];
        if (i < MAX_LINES) {
            CurrentScreen.col[i] = EditScreen.col[i];
            CurrentScreen.line_size[i] = EditScreen.line_size[i];
            CurrentScreen.new_line[i] = EditScreen.new_line[i];
        }
    }

    if (CurrentScreen.col[CurrentScreen.row] != 0) 
    {
        CurrentScreen.new_line[++CurrentScreen.row] = true;
    }
    
    CursorPosition(SCREEN_OFFSET);
}
#pragma optimize("", on)

#pragma optimize("", off)
void RestoreNormalSceen()
{
    for (int i = 0; i < MAX_OFFSET; i++)
    {
        gVideo[i].c = ' ';
        CurrentScreen.Buffer[i] = ' ';
    }

    text_color = 10;

    CurrentScreen.row = NormalScreen.row;

    for (int i = 0; i < MAX_OFFSET; i++)
    {
        gVideo[i].color = text_color;
        gVideo[i].c = NormalScreen.Buffer[i];
        CurrentScreen.Buffer[i] = NormalScreen.Buffer[i];

        if (i < MAX_LINES) 
        {
            CurrentScreen.col[i] = NormalScreen.col[i];
            CurrentScreen.line_size[i] = NormalScreen.line_size[i];
            CurrentScreen.new_line[i] = NormalScreen.new_line[i];
        }
    }

    CursorPosition(SCREEN_OFFSET);
}
#pragma optimize("", on)

#pragma optimize("", off)
void EnterMode(CONSOLE_MODE mode)
{
    ConsoleMode = mode;

    if (mode == EDIT_MODE) 
    {
        SaveNormalScreen();

        //RestoreEditScreen();
        RestoreScreenState(&EditScreen);
    }
    else 
    {
        SaveEditScreen();

        RestoreScreenState(&NormalScreen);

        //RestoreNormalSceen();
    }
}
#pragma optimize("", on)

void PutCharExt(KEYCODE C) 
{
    // Handling arrow keys
    if (C == KEY_DELETE && CurrentScreen.col[CurrentScreen.row] < CurrentScreen.line_size[CurrentScreen.row]) {


        // shift left
        for (int i = CurrentScreen.col[CurrentScreen.row]; i < CurrentScreen.line_size[CurrentScreen.row]; i++) {
            if (i == CurrentScreen.line_size[CurrentScreen.row] - 1)
            {
                gVideo[CurrentScreen.row * MAX_COLUMNS + i].color = text_color;
                gVideo[CurrentScreen.row * MAX_COLUMNS + i].c = ' ';

                CurrentScreen.Buffer[CurrentScreen.row * MAX_COLUMNS + i] = ' ';
            }
            else
            {
                gVideo[CurrentScreen.row * MAX_COLUMNS + i].color = text_color;
                gVideo[CurrentScreen.row * MAX_COLUMNS + i].c = gVideo[CurrentScreen.row * MAX_COLUMNS + i + 1].c;

                CurrentScreen.Buffer[CurrentScreen.row * MAX_COLUMNS + i] = CurrentScreen.Buffer[CurrentScreen.row * MAX_COLUMNS + i + 1];
            }

        }


        CLIBuffer[CurrentScreen.line_size[CurrentScreen.row]] = '\0';//here was current offeset
        CursorPosition(SCREEN_OFFSET);
        CurrentScreen.line_size[CurrentScreen.row]--;
    }
    else if (C == KEY_UP && CurrentScreen.row > 0 && ConsoleMode == EDIT_MODE) {

        CurrentScreen.row--;
        if (CurrentScreen.line_size[CurrentScreen.row] > CurrentScreen.col[CurrentScreen.row + 1])
            CurrentScreen.col[CurrentScreen.row] = CurrentScreen.col[CurrentScreen.row + 1];
        else if (CurrentScreen.new_line[CurrentScreen.row] == false)
            CurrentScreen.col[CurrentScreen.row] = CurrentScreen.line_size[CurrentScreen.row];
        

    }
    else if (C == KEY_DOWN && CurrentScreen.row < MAX_LINES - 1 && ConsoleMode == EDIT_MODE) {
        if (CurrentScreen.new_line[CurrentScreen.row + 1] == true)
        {
            CurrentScreen.row++;

            if (CurrentScreen.line_size[CurrentScreen.row] > CurrentScreen.col[CurrentScreen.row - 1])
                CurrentScreen.col[CurrentScreen.row] = CurrentScreen.col[CurrentScreen.row - 1];
        }
    }
    else if (C == KEY_LEFT && CurrentScreen.col[CurrentScreen.row] > 0) {
        CurrentScreen.col[CurrentScreen.row]--;
    }
    else if (C == KEY_RIGHT && CurrentScreen.col[CurrentScreen.row] < MAX_COLUMNS - 1) { // end of console
        if (CurrentScreen.col[CurrentScreen.row] < CurrentScreen.line_size[CurrentScreen.row]) {
            CurrentScreen.col[CurrentScreen.row]++;
        }
        else if (CurrentScreen.row < MAX_LINES - 1 && ConsoleMode == EDIT_MODE)
        {
            if (CurrentScreen.new_line[CurrentScreen.row + 1] == true) {
                CurrentScreen.row++;

                CurrentScreen.col[CurrentScreen.row] = 0;
            }

        }
    }
    CursorPosition(SCREEN_OFFSET);
}

void PutCharStd(KEYCODE C)
{
    if (C == ENTER_KEY || C == ENTER_KEY2) {

        CurrentScreen.row++;
        CurrentScreen.new_line[CurrentScreen.row] = true;
        CursorPosition(SCREEN_OFFSET);

        if (ConsoleMode == NORMAL_MODE) {
            CLIBuffer[CurrentScreen.line_size[CurrentScreen.row - 1]] = '\0';
            ParseCommand(CLIBuffer, CurrentScreen.line_size[CurrentScreen.row - 1]);

            CurrentScreen.line_size[CurrentScreen.row] = 0;
        }
        
    }
    else if (C == BACKSPACE_KEY && CurrentScreen.col[CurrentScreen.row] > 0) 
    {
        CurrentScreen.col[CurrentScreen.row]--;

        // shift left
        for (int i = CurrentScreen.col[CurrentScreen.row]; i < CurrentScreen.line_size[CurrentScreen.row]; i++) 
        {
            if (i == CurrentScreen.line_size[CurrentScreen.row] - 1)
            {
                gVideo[CurrentScreen.row * MAX_COLUMNS + i].color = text_color;
                gVideo[CurrentScreen.row * MAX_COLUMNS + i].c = ' ';

                CurrentScreen.Buffer[CurrentScreen.row * MAX_COLUMNS + i] = ' ';
            }
            else
            {
                gVideo[CurrentScreen.row * MAX_COLUMNS + i].color = text_color;
                gVideo[CurrentScreen.row * MAX_COLUMNS + i].c = gVideo[CurrentScreen.row * MAX_COLUMNS + i + 1].c;

                CurrentScreen.Buffer[CurrentScreen.row * MAX_COLUMNS + i] = CurrentScreen.Buffer[CurrentScreen.row * MAX_COLUMNS + i + 1];
            }
        }

        CLIBuffer[CurrentScreen.line_size[CurrentScreen.row]] = '\0';
        CursorPosition(SCREEN_OFFSET);
        CurrentScreen.line_size[CurrentScreen.row]--;
    }
    else if (C == KEY_ESCAPE && ConsoleMode == EDIT_MODE) 
    {
        EnterMode(NORMAL_MODE);
    }
    else 
    {
        gVideo[SCREEN_OFFSET].color = text_color;

        if (is_value(C) && CurrentScreen.line_size[CurrentScreen.row] < MAX_COLUMNS)
        {
            CurrentScreen.line_size[CurrentScreen.row]++;

            // shift right
            for (int i = CurrentScreen.line_size[CurrentScreen.row] - 1; i > CurrentScreen.col[CurrentScreen.row]; i--) 
            {
                gVideo[CurrentScreen.row * MAX_COLUMNS + i].c = gVideo[CurrentScreen.row * MAX_COLUMNS + i - 1].c;

                CurrentScreen.Buffer[CurrentScreen.row * MAX_COLUMNS + i] = CurrentScreen.Buffer[CurrentScreen.row * MAX_COLUMNS + i - 1];
            }

            gVideo[SCREEN_OFFSET].c = (char)C;

            CurrentScreen.Buffer[SCREEN_OFFSET] = (char)C;
           
            CLIBuffer[CurrentScreen.col[CurrentScreen.row]++] = (char)C;
        }

        if (CurrentScreen.col[CurrentScreen.row] >= MAX_COLUMNS) {
            CurrentScreen.row++;
            CurrentScreen.new_line[CurrentScreen.row] = true;
            CurrentScreen.col[CurrentScreen.row] = 0;
            if (ConsoleMode == NORMAL_MODE) CurrentScreen.line_size[CurrentScreen.row] = 0;
        }

        if (CurrentScreen.row >= MAX_LINES) {
            ClearScreen();
            CurrentScreen.row = 0;
            CurrentScreen.new_line[0] = true;
            CurrentScreen.line_size[CurrentScreen.row] = 0;
            CurrentScreen.col[CurrentScreen.row] = 0;
        }

        CursorPosition(SCREEN_OFFSET);
    }
}


void ScreenDisplay(char* logBuffer, int color)
{
    int i, currentColumn = CurrentScreen.col[CurrentScreen.row], currentRow = CurrentScreen.row;

    if (logBuffer == NULL)
        return;

    // Write the logBuffer to video memory
    for (i = 0; logBuffer[i] != '\0' && i < MAX_OFFSET; i++)
    {
        if (logBuffer[i] == '\n')
        {
            currentRow++;
            currentColumn = 0; 

            if (currentRow >= MAX_LINES)
            {
                ClearScreen();
                currentRow = 0; 
            }
        }
        else
        {
            int pos = currentRow * MAX_COLUMNS + currentColumn; 

            if (pos < MAX_OFFSET)
            {
                gVideo[pos].c = logBuffer[i];
                gVideo[pos].color = color;
                if(ConsoleMode == NORMAL_MODE) CurrentScreen.Buffer[pos] = logBuffer[i];

                currentColumn++;
                
                if (currentColumn >= MAX_COLUMNS)
                {
                    currentColumn = 0; 
                    currentRow++;
                }

               
                if (currentRow >= MAX_LINES)
                {
                    ClearScreen(); 
                    currentRow = 0; 
                }
            }
        }
    }

    CurrentScreen.row++;

    CursorPosition(SCREEN_OFFSET);
}

