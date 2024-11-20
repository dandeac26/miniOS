#include "screen.h"


#define SCREEN_OFFSET (CurrentScreen.row * MAX_COLUMNS + CurrentScreen.col[CurrentScreen.row])


static PSCREEN gVideo = (PSCREEN)(0x000B8000);
static char CLIBuffer[82];


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


void IntBufferInit(int* buf, size_t size, int value) 
{
    for (int i = 0; i < size; i++) 
    {
        buf[i] = value;
    }
}


#pragma optimize("", off)
void CharBufferInit(char* buf, size_t size, char value) 
{
    for (int i = 0; i < size; i++)
    {
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

 
    CursorPosition(SCREEN_OFFSET);
}
#pragma optimize("", on)


#pragma optimize("", off)
void SaveScreenState(SCREEN_STATE* state)
{
    state->row = CurrentScreen.row;

    for (int i = 0; i < MAX_OFFSET; i++)
    {
        state->Buffer[i] = CurrentScreen.Buffer[i];

        if (i < MAX_LINES)
        {
            state->col[i] = CurrentScreen.col[i];
            state->line_size[i] = CurrentScreen.line_size[i];
            state->new_line[i] = CurrentScreen.new_line[i];
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
void EnterMode(CONSOLE_MODE mode)
{
    ConsoleMode = mode;

    if (mode == EDIT_MODE) 
    {
        SaveScreenState(&NormalScreen);

        RestoreScreenState(&EditScreen);
    }
    else 
    {
        SaveScreenState(&EditScreen);

        RestoreScreenState(&NormalScreen);
    }
}
#pragma optimize("", on)


void PutCharExt(KEYCODE C) 
{
    // Handling arrow keys
    if (C == KEY_DELETE && CurrentScreen.col[CurrentScreen.row] < CurrentScreen.line_size[CurrentScreen.row]) 
    {
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


        CLIBuffer[CurrentScreen.line_size[CurrentScreen.row]] = '\0';//here was current offeset
        CursorPosition(SCREEN_OFFSET);
        CurrentScreen.line_size[CurrentScreen.row]--;
    }
    else if (C == KEY_UP && CurrentScreen.row > 0 && ConsoleMode == EDIT_MODE) 
    {
        CurrentScreen.row--;
        if (CurrentScreen.line_size[CurrentScreen.row] > CurrentScreen.col[CurrentScreen.row + 1])
        {
            CurrentScreen.col[CurrentScreen.row] = CurrentScreen.col[CurrentScreen.row + 1];
        }
        else if (CurrentScreen.new_line[CurrentScreen.row] == false)
        {
            CurrentScreen.col[CurrentScreen.row] = CurrentScreen.line_size[CurrentScreen.row];
        }
    }
    else if (C == KEY_DOWN && CurrentScreen.row < MAX_LINES - 1 && ConsoleMode == EDIT_MODE) 
    {
        if (CurrentScreen.new_line[CurrentScreen.row + 1] == true)
        {
            CurrentScreen.row++;

            if (CurrentScreen.line_size[CurrentScreen.row] > CurrentScreen.col[CurrentScreen.row - 1])
                CurrentScreen.col[CurrentScreen.row] = CurrentScreen.col[CurrentScreen.row - 1];
        }
    }
    else if (C == KEY_LEFT && CurrentScreen.col[CurrentScreen.row] > 0) 
    {
        CurrentScreen.col[CurrentScreen.row]--;
    }
    else if (C == KEY_RIGHT && CurrentScreen.col[CurrentScreen.row] < MAX_COLUMNS - 1) 
    { // end of console
        if (CurrentScreen.col[CurrentScreen.row] < CurrentScreen.line_size[CurrentScreen.row]) 
        {
            CurrentScreen.col[CurrentScreen.row]++;
        }
        else if (CurrentScreen.row < MAX_LINES - 1 && ConsoleMode == EDIT_MODE)
        {
            if (CurrentScreen.new_line[CurrentScreen.row + 1] == true) 
            {
                CurrentScreen.row++;

                CurrentScreen.col[CurrentScreen.row] = 0;
            }

        }
    }
    CursorPosition(SCREEN_OFFSET);
}

void PutCharStd(KEYCODE C)
{
    if (C == ENTER_KEY || C == ENTER_KEY2) 
    {

        CurrentScreen.row++;
        CurrentScreen.new_line[CurrentScreen.row] = true;
        CursorPosition(SCREEN_OFFSET);

        if (ConsoleMode == NORMAL_MODE) 
        {
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

        if (CurrentScreen.col[CurrentScreen.row] >= MAX_COLUMNS) 
        {
            CurrentScreen.row++;
            CurrentScreen.new_line[CurrentScreen.row] = true;
            CurrentScreen.col[CurrentScreen.row] = 0;
            if (ConsoleMode == NORMAL_MODE) CurrentScreen.line_size[CurrentScreen.row] = 0;
        }

        if (CurrentScreen.row >= MAX_LINES) 
        {
            ClearScreen();
            CurrentScreen.row = 0;
            CurrentScreen.new_line[0] = true;
            CurrentScreen.line_size[CurrentScreen.row] = 0;
            CurrentScreen.col[CurrentScreen.row] = 0;
        }

        CursorPosition(SCREEN_OFFSET);
    }
}

#include "ata_commands.h"

#pragma optimize("", off)
void testfnc() 
{

    int drive = 0;

    BYTE buffer[512] = { 0 }; // MBR is exactly one sector (512 bytes)

    //// Read the first sector using the ATA command
    ata_send_command(drive, ATA_CMD_READ_SECTORS, 0, &buffer, 1);

    //// Clear the screen buffer before displaying
    //for (int i = 0; i < MAX_OFFSET; i++) {
    //    gVideo[i].color = 10;
    //    gVideo[i].c = ' ';
    //    CurrentScreen.Buffer[i] = ' ';
    //}

    //// Copy the buffer directly into the screen_buffer
    int offset = 0;
    for (int i = 0; i < 512; i++) {
       // Fill the screen buffer with raw data from the disk sector
        if (offset < MAX_OFFSET) {
            gVideo[offset].color = 10;
            gVideo[offset].c =(char)buffer[i];
            CurrentScreen.Buffer[offset] = (char)buffer[i];
            offset++;
            //screen_buffer[offset++] = buffer[i];
        }
    }
    //LogSerialAndScreen("read data: %X", screen_buffer);


}
#pragma optimize("", on)

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

