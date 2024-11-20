#include "screen.h"

#define BUFF_ROW (CurrentScreen.row + CurrentScreen.view_offset)
#define SCREEN_OFFSET (CurrentScreen.row * MAX_COLUMNS + CurrentScreen.col[BUFF_ROW])
#define BUFFER_OFFSET ((CurrentScreen.row + CurrentScreen.view_offset) * MAX_COLUMNS + CurrentScreen.col[BUFF_ROW])

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

    IntBufferInit(CurrentScreen.col, TOTAL_MAX_LINES, 0);
    CurrentScreen.row = 0;
    CurrentScreen.view_offset = 0;
    IntBufferInit(CurrentScreen.line_size, TOTAL_MAX_LINES, 0);
    IntBufferInit(CurrentScreen.new_line, TOTAL_MAX_LINES, false);
    CharBufferInit(CurrentScreen.Buffer, TOTAL_OFFSET, ' ');

    IntBufferInit(NormalScreen.col, TOTAL_MAX_LINES, 0);
    NormalScreen.row = 0;
    NormalScreen.view_offset = 0;
    IntBufferInit(NormalScreen.line_size, TOTAL_MAX_LINES, 0);
    IntBufferInit(NormalScreen.new_line, TOTAL_MAX_LINES, false);
    CharBufferInit(NormalScreen.Buffer, TOTAL_OFFSET, ' ');

    IntBufferInit(EditScreen.col, TOTAL_MAX_LINES, 0);
    EditScreen.row = 0;
    EditScreen.view_offset = 0;
    IntBufferInit(EditScreen.line_size, TOTAL_MAX_LINES, 0);
    IntBufferInit(EditScreen.new_line, TOTAL_MAX_LINES, false);
    CharBufferInit(EditScreen.Buffer, TOTAL_OFFSET, ' ');
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

    for (i = MAX_OFFSET; i < TOTAL_OFFSET; i++)
    {
        CurrentScreen.Buffer[i] = ' ';
    }

    IntBufferInit(CurrentScreen.col, TOTAL_MAX_LINES, 0);
    CurrentScreen.row = 0;
    CurrentScreen.view_offset = 0;
    IntBufferInit(CurrentScreen.line_size, TOTAL_MAX_LINES, 0);
    IntBufferInit(CurrentScreen.new_line, TOTAL_MAX_LINES, false);

 
    CursorPosition(SCREEN_OFFSET);
}
#pragma optimize("", on)


#pragma optimize("", off)
void SaveScreenState(SCREEN_STATE* state)
{
    state->row = CurrentScreen.row;
    state->view_offset = CurrentScreen.view_offset;

    for (int i = 0; i < TOTAL_OFFSET; i++)
    {
        state->Buffer[i] = CurrentScreen.Buffer[i];

        if (i < TOTAL_MAX_LINES)
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
    for (int i = MAX_OFFSET; i < TOTAL_OFFSET; i++) 
    {
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
    CurrentScreen.view_offset = state->view_offset;

    for (int i = 0; i < TOTAL_OFFSET; i++)
    {
        if (i < MAX_OFFSET) 
        {
            gVideo[i].color = text_color;
            gVideo[i].c = state->Buffer[CurrentScreen.view_offset * MAX_COLUMNS + i];
        }
       
        CurrentScreen.Buffer[i] = state->Buffer[i];

        if (i < TOTAL_MAX_LINES)
        {
            CurrentScreen.col[i] = state->col[i];
            CurrentScreen.line_size[i] = state->line_size[i];
            CurrentScreen.new_line[i] = state->new_line[i];
        }
    }

    if (ConsoleMode == EDIT_MODE) {
        if (CurrentScreen.col[CurrentScreen.row + CurrentScreen.view_offset] != 0)
        {
            if (CurrentScreen.row > MAX_LINES)
            {
                CurrentScreen.view_offset++;
            }
            else
            {
                CurrentScreen.row++;
            }
            CurrentScreen.new_line[CurrentScreen.row + CurrentScreen.view_offset] = true; //bug if current row is at end
            /// ADD CHECK IF 
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


void UpScroll(int num_rows)
{
    CurrentScreen.view_offset--;

    for (int i = 0; i < MAX_OFFSET; i++)
    {
        gVideo[i].color = text_color;
        gVideo[i].c = CurrentScreen.Buffer[CurrentScreen.view_offset * MAX_COLUMNS + i];
    }
    CurrentScreen.row++;
    CursorPosition(SCREEN_OFFSET);
}

void DownScroll(int num_rows)
{
    CurrentScreen.view_offset++;

    for (int i = 0; i < MAX_OFFSET; i++)
    {
        gVideo[i].color = text_color;
        gVideo[i].c = CurrentScreen.Buffer[CurrentScreen.view_offset*MAX_COLUMNS + i];
    }
    CurrentScreen.row--;
    CursorPosition(SCREEN_OFFSET);
}



void PutCharExt(KEYCODE C) 
{
    // Handling arrow keys
    if (C == KEY_DELETE && CurrentScreen.col[BUFF_ROW] < CurrentScreen.line_size[BUFF_ROW])
    {
        // shift left
        for (int i = CurrentScreen.col[BUFF_ROW]; i < CurrentScreen.line_size[BUFF_ROW]; i++)
        {
            if (i == CurrentScreen.line_size[BUFF_ROW] - 1)
            {
                gVideo[CurrentScreen.row * MAX_COLUMNS + i].color = text_color;
                gVideo[CurrentScreen.row * MAX_COLUMNS + i].c = ' ';

                CurrentScreen.Buffer[BUFF_ROW * MAX_COLUMNS + i] = ' ';
            }
            else
            {
                gVideo[CurrentScreen.row * MAX_COLUMNS + i].color = text_color;
                gVideo[CurrentScreen.row * MAX_COLUMNS + i].c = gVideo[CurrentScreen.row * MAX_COLUMNS + i + 1].c;

                CurrentScreen.Buffer[BUFF_ROW * MAX_COLUMNS + i] = CurrentScreen.Buffer[BUFF_ROW * MAX_COLUMNS + i + 1];
            }
        }


        CLIBuffer[CurrentScreen.line_size[BUFF_ROW]] = '\0';//here was current offeset
        CursorPosition(SCREEN_OFFSET);
        CurrentScreen.line_size[BUFF_ROW]--;
    }
   
    else if (C == KEY_UP && CurrentScreen.row > 0 && ConsoleMode == EDIT_MODE) 
    {
        CurrentScreen.row--;
        if (CurrentScreen.line_size[BUFF_ROW] > CurrentScreen.col[BUFF_ROW + 1])
        {
            CurrentScreen.col[BUFF_ROW] = CurrentScreen.col[BUFF_ROW + 1];
        }
        else if (CurrentScreen.new_line[BUFF_ROW] == false)
        {
            CurrentScreen.col[BUFF_ROW] = CurrentScreen.line_size[BUFF_ROW];
        }
    }
    else if (C == KEY_DOWN && CurrentScreen.row < MAX_LINES - 1 && ConsoleMode == EDIT_MODE) 
    {
        if (CurrentScreen.new_line[BUFF_ROW + 1] == true)
        {
            CurrentScreen.row++;

            if (CurrentScreen.line_size[BUFF_ROW] > CurrentScreen.col[BUFF_ROW - 1])
                CurrentScreen.col[BUFF_ROW] = CurrentScreen.col[BUFF_ROW - 1];
        }
    }
    else if (C == KEY_LEFT && CurrentScreen.col[BUFF_ROW] > 0) 
    {
        CurrentScreen.col[BUFF_ROW]--;
    }
    else if (C == KEY_RIGHT && CurrentScreen.col[BUFF_ROW] < MAX_COLUMNS - 1) 
    { // end of console
        if (CurrentScreen.col[BUFF_ROW] < CurrentScreen.line_size[BUFF_ROW]) 
        {
            CurrentScreen.col[BUFF_ROW]++;
        }
        else if (CurrentScreen.row < MAX_LINES - 1 && ConsoleMode == EDIT_MODE)
        {
            if (CurrentScreen.new_line[BUFF_ROW + 1] == true)
            {
                CurrentScreen.row++;

                CurrentScreen.col[BUFF_ROW] = 0;
            }

        }
    }
    CursorPosition(SCREEN_OFFSET);
}

void PutCharStd(KEYCODE C)
{
    if (C == ENTER_KEY || C == ENTER_KEY2) 
    {
        if (CurrentScreen.row + 1 > MAX_LINES) {
            DownScroll(1);
        }
        CurrentScreen.row++;
        CurrentScreen.new_line[BUFF_ROW] = true;
        CursorPosition(SCREEN_OFFSET);

        if (ConsoleMode == NORMAL_MODE) 
        {
            CLIBuffer[CurrentScreen.line_size[BUFF_ROW - 1]] = '\0';
            ParseCommand(CLIBuffer, CurrentScreen.line_size[BUFF_ROW - 1]);

            CurrentScreen.line_size[BUFF_ROW] = 0;
        }
        
    }
    else if (C == KEY_MINUS && BUFF_ROW > 0)
    {
        UpScroll(1);
    }
    else if (C == KEY_EQUAL && BUFF_ROW < TOTAL_MAX_LINES)
    {
        DownScroll(1);
    }
    else if (C == BACKSPACE_KEY && CurrentScreen.col[BUFF_ROW] > 0) 
    {
        CurrentScreen.col[BUFF_ROW]--;

        // shift left
        for (int i = CurrentScreen.col[BUFF_ROW]; i < CurrentScreen.line_size[BUFF_ROW]; i++) 
        {
            if (i == CurrentScreen.line_size[BUFF_ROW] - 1)
            {
                gVideo[CurrentScreen.row * MAX_COLUMNS + i].color = text_color;
                gVideo[CurrentScreen.row * MAX_COLUMNS + i].c = ' ';

                CurrentScreen.Buffer[BUFF_ROW * MAX_COLUMNS + i] = ' ';
            }
            else
            {
                gVideo[CurrentScreen.row * MAX_COLUMNS + i].color = text_color;
                gVideo[CurrentScreen.row * MAX_COLUMNS + i].c = gVideo[CurrentScreen.row * MAX_COLUMNS + i + 1].c;

                CurrentScreen.Buffer[BUFF_ROW * MAX_COLUMNS + i] = CurrentScreen.Buffer[BUFF_ROW * MAX_COLUMNS + i + 1];
            }
        }

        CLIBuffer[CurrentScreen.line_size[BUFF_ROW]] = '\0';
        CursorPosition(SCREEN_OFFSET);
        CurrentScreen.line_size[BUFF_ROW]--;
    }
    else if (C == KEY_ESCAPE && ConsoleMode == EDIT_MODE) 
    {
        EnterMode(NORMAL_MODE);
    }
    else 
    {
        gVideo[SCREEN_OFFSET].color = text_color;

        if (is_value(C) && CurrentScreen.line_size[BUFF_ROW] < MAX_COLUMNS)
        {
            CurrentScreen.line_size[BUFF_ROW]++;

            // shift right
            for (int i = CurrentScreen.line_size[BUFF_ROW] - 1; i > CurrentScreen.col[BUFF_ROW]; i--) 
            {
                gVideo[CurrentScreen.row * MAX_COLUMNS + i].c = gVideo[CurrentScreen.row * MAX_COLUMNS + i - 1].c;

                CurrentScreen.Buffer[BUFF_ROW * MAX_COLUMNS + i] = CurrentScreen.Buffer[BUFF_ROW * MAX_COLUMNS + i - 1];
            }

            gVideo[SCREEN_OFFSET].c = (char)C;

            CurrentScreen.Buffer[BUFFER_OFFSET] = (char)C;
           
            CLIBuffer[CurrentScreen.col[BUFF_ROW]++] = (char)C;
        }

        if (CurrentScreen.col[BUFF_ROW] >= MAX_COLUMNS) 
        {
            CurrentScreen.row++;
            CurrentScreen.new_line[BUFF_ROW] = true;
            CurrentScreen.col[BUFF_ROW] = 0;
            if (ConsoleMode == NORMAL_MODE) CurrentScreen.line_size[BUFF_ROW] = 0;
        }

        if (CurrentScreen.row >= MAX_LINES) 
        {
            //ClearScreen();
            //CurrentScreen.row = 0;
            //CurrentScreen.new_line[0] = true;
            //CurrentScreen.line_size[BUFF_ROW] = 0;
            //CurrentScreen.col[BUFF_ROW] = 0;
            DownScroll(1);
        }

        CursorPosition(SCREEN_OFFSET);
    }
}

char getHexChar(size_t offset, int nibbleIndex) {
    // Ensure nibbleIndex is between 0 and 7 (for an 8-digit hex value)
    if (nibbleIndex < 0 || nibbleIndex > 7) {
        return ' '; // Return a space if the index is invalid (optional error handling)
    }

    // Extract the specific nibble using the bit-shift and mask technique
    return "0123456789ABCDEF"[(offset >> (4 * (7 - nibbleIndex))) & 0xF];
}


#pragma optimize("", off)
void PutHexViewString(char* buffer, size_t size)
{
    size_t i, j;
    DWORD offset;
    char hex_byte[4]; // 2 hex digits + space
    char ascii_byte;
 
    // iterate buffer in chunks of 16 bytes
    for (i = 0; i < size; i += 16) {
        
        offset = i; // print the offset (address) in hex

        // Print offset (8 characters for hex, 1 space)
        for (j = 0; j < 8; j++) 
        {
            if (offset > 0) {
                int iterator1 = (CurrentScreen.row + (j / MAX_COLUMNS)) * MAX_COLUMNS +
                    (CurrentScreen.col[BUFF_ROW] + (j % MAX_COLUMNS));
                gVideo[iterator1].color = 10;
                gVideo[iterator1].c = getHexChar(offset, j);
                CurrentScreen.Buffer[iterator1] = getHexChar(offset, j);

                offset &= ~(0xF << (4 * (7 - j))); // clear current hex digit
            }
        }

        // print the hexadecimal values (16 bytes - 2 hex digits each)
        for (j = 0; j < 16 && (i + j) < size; j++) 
        {
            unsigned char byte = buffer[i + j];

            // Print the byte in hex format
            hex_byte[0] = "0123456789ABCDEF"[byte >> 4];
            hex_byte[1] = "0123456789ABCDEF"[byte & 0x0F];
            hex_byte[2] = ' ';
            hex_byte[3] = '\0';

            // hex_byte (2 hex digits and space)
            for (int k = 0; k < 3; k++) 
            {
                int iteratorr = (CurrentScreen.row + ((8 + j * 3 + k) / MAX_COLUMNS)) * MAX_COLUMNS +
                    (CurrentScreen.col[BUFF_ROW] + ((8 + j * 3 + k) % MAX_COLUMNS));

                gVideo[iteratorr].color = 10;
                gVideo[iteratorr].c = hex_byte[k];
                CurrentScreen.Buffer[iteratorr] = hex_byte[k];
            }
        }

        int iteratorr = (CurrentScreen.row + (8 + 16 * 3) / MAX_COLUMNS) * MAX_COLUMNS +
            (CurrentScreen.col[BUFF_ROW] + (8 + 16 * 3) % MAX_COLUMNS);
        
        gVideo[iteratorr].color = 10;
        gVideo[iteratorr].c = ' ';
        CurrentScreen.Buffer[iteratorr] = ' ';

        // ascii representation (printable characters or '.' for non-printable)
        for (j = 0; j < 16 && (i + j) < size; j++) 
        {
            ascii_byte = buffer[i + j];

            if (ascii_byte < 32 || ascii_byte > 126) {  // Non-printable characters
                ascii_byte = '.';
            }

            int iteratorr = (CurrentScreen.row + (8 + 16 * 3 + 1 + j) / MAX_COLUMNS) * MAX_COLUMNS +
                (CurrentScreen.col[BUFF_ROW] + (8 + 16 * 3 + 1 + j) % MAX_COLUMNS);
            
            gVideo[iteratorr].color = 10;
            gVideo[iteratorr].c = ascii_byte;
            CurrentScreen.Buffer[iteratorr] = ascii_byte;
        }

 
        CurrentScreen.row = CurrentScreen.row + ((8 + 16 * 3 + 1 + 16) / MAX_COLUMNS) + 1;

        if (CurrentScreen.row >= MAX_LINES) 
        {
            DownScroll(1);
            /*CurrentScreen.row = 0;
            CurrentScreen.col[BUFF_ROW] = 0;
            CursorPosition(SCREEN_OFFSET);
            break;*/
        }
        CurrentScreen.col[BUFF_ROW] = 0;

        CursorPosition(SCREEN_OFFSET);
    }
}
#pragma optimize("", on)

#pragma optimize("", off)
void PutString(char* buffer, size_t size)
{
    size_t i;
    for ( i = 0; i < size && i < MAX_OFFSET; i++) 
    {
        gVideo[(CurrentScreen.row + (i / MAX_COLUMNS)) * MAX_COLUMNS + (CurrentScreen.col[BUFF_ROW] + (i % MAX_COLUMNS))].color = 10;
        gVideo[(CurrentScreen.row + (i / MAX_COLUMNS)) * MAX_COLUMNS + (CurrentScreen.col[BUFF_ROW] + (i % MAX_COLUMNS))].c = (char)buffer[i];
        CurrentScreen.Buffer[(CurrentScreen.row + (i / MAX_COLUMNS)) * MAX_COLUMNS + (CurrentScreen.col[BUFF_ROW] + (i % MAX_COLUMNS))] = (char)buffer[i];
    }
    CurrentScreen.row = CurrentScreen.row + (i / MAX_COLUMNS)+1;
    CurrentScreen.col[BUFF_ROW] = 0;
    CursorPosition(SCREEN_OFFSET);
}
#pragma optimize("", on)


void ScreenDisplay(char* logBuffer, int color)
{
    int i, currentColumn = CurrentScreen.col[BUFF_ROW], currentRow = CurrentScreen.row;

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

