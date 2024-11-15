#include "screen.h"

static PSCREEN gVideo = (PSCREEN)(0x000B8000);
static char CLIBuffer[82];

#define SCREEN_OFFSET (current_row * MAX_COLUMNS + current_line_offset)

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

void HelloBoot()
{
    int i, len;
	char boot[] = "Hello Boot! Greetings from C... lol";

	len = 0;
	while (boot[len] != 0)
	{
		len++;
	}

	for (i = 0; (i < len) && (i < MAX_OFFSET); i++)
	{
		gVideo[i].color = 10;
		gVideo[i].c = boot[i];
	}
    CursorPosition(i);
}

void ClearScreen()
{
    int i;

    for (i = 0; i < MAX_OFFSET; i++)
    {
        gVideo[i].color = 10;
        gVideo[i].c = ' ';
    }
  /*  current_line_offset = 0;
    CursorPosition(SCREEN_OFFSET);*/
    CursorMove(0, 0);
}

void PutChar(KEYCODE C, int is_ext)
{
    // Handling regular keys when is_ext == 0
    if (is_ext == 0) {


        if (C == ENTER_KEY || C == ENTER_KEY2) {
            CLIBuffer[current_line_offset] = '\0';
            ParseCommand(CLIBuffer, strlen(CLIBuffer));
            current_row++;
            current_line_offset = 0;
            CursorPosition(SCREEN_OFFSET);
            line_size = 0;
        }
        else if (C == BACKSPACE_KEY && current_line_offset > 0) {
            current_line_offset--;

            /*gVideo[SCREEN_OFFSET].color = 10;
            gVideo[SCREEN_OFFSET].c = ' ';*/
            
            // shift left
            for (int i = current_line_offset; i < line_size; i++) {
                if (i == line_size - 1)
                {
                    gVideo[current_row * MAX_COLUMNS + i].color = 10;
                    gVideo[current_row * MAX_COLUMNS + i].c = ' ';
                }
                else {
                    gVideo[current_row * MAX_COLUMNS + i].color = 10;
                    gVideo[current_row * MAX_COLUMNS + i].c = gVideo[current_row * MAX_COLUMNS + i + 1].c;
                }
               
            }

            
            CLIBuffer[current_line_offset] = '\0';
            CursorPosition(SCREEN_OFFSET);
            line_size--;
            
        }
        else {
            gVideo[SCREEN_OFFSET].color = 10;

            if (C == KEY_SPACE || (C >= 'A' && C <= 'Z') || (C >= 'a' && C <= 'z') || (C >= '0' && C <= '9') || (C == '.' || C == ',' || C == ';' || C == '-'))
            {
                gVideo[SCREEN_OFFSET].c = (char)C;
                CLIBuffer[current_line_offset++] = (char)C;
                line_size++;
            }

            if (current_line_offset >= MAX_COLUMNS) {
                current_row++;
                current_line_offset = 0;
                line_size = 0;
            }

            if (current_row >= MAX_LINES) {
                ClearScreen();
                current_row = 0;
            }

            CursorPosition(SCREEN_OFFSET);
        }
    }

    else if (is_ext == 1) {
        // Handling arrow keys
        if (C == KEY_UP && current_row > 0 && ConsoleMode == EDIT_MODE) {
            current_row--;
        }
        else if (C == KEY_DOWN && current_row < MAX_LINES - 1 && ConsoleMode == EDIT_MODE) {
            current_row++;
        }
        else if (C == KEY_LEFT && current_line_offset > 0) {
            current_line_offset--;
        }
        else if (C == KEY_RIGHT && current_line_offset < MAX_COLUMNS - 1) { // end of console
            if (ConsoleMode == NORMAL_MODE && current_line_offset < line_size) {
                current_line_offset++;
            }
            else if (ConsoleMode == EDIT_MODE) {
                current_line_offset++;
            }
            
        }
        CursorPosition(SCREEN_OFFSET);
    }
}



//void PutChar(KEYCODE C, int is_ext)
//{
//    if (is_ext == 0) {
//
//
//        gVideo[SCREEN_OFFSET].color = 10;
//
//        if (C == KEY_SPACE) {
//            gVideo[SCREEN_OFFSET].c = C;
//            ++current_line_offset;
//        }
//        else {
//            gVideo[SCREEN_OFFSET].c = (char)C;
//            ++current_line_offset;
//        }
//        
//
//        if (current_line_offset >= MAX_COLUMNS) {
//            current_row++;
//            current_line_offset = 0;
//        }
//
//        if (current_row >= MAX_LINES)
//        {
//            ClearScreen(); // Clear the screen, or implement scrolling logic if desired
//            current_row = 0; // Reset to the first row
//        }
//
//        CLIBuffer[current_line_offset] = C;
//        
//        CursorPosition(SCREEN_OFFSET);
//    }
//    else if ( C == ENTER_KEY2 || C == ENTER_KEY)
//    {
//        CLIBuffer[current_line_offset] = '\0';
//        //
//        ParseCommand(CLIBuffer, strlen(CLIBuffer));
//        
//        current_row++;
//        current_line_offset = 0;
//
//        last_enter_offset = SCREEN_OFFSET;
//        CursorPosition(SCREEN_OFFSET);
//        
//        //ClearScreen();
//       /* ClearScreen();
//        char MSG[] = "You typed command: ";
//        for (int i = 0; (i < strlen(MSG)) && (i < MAX_OFFSET); i++) {
//            gVideo[i].color = 10;
//            gVideo[i].c = MSG[i];
//        }
//        current_line_offset += strlen(MSG)+1;
//        CursorPosition(current_line_offset);
//        enter_was_typed = 1;*/
//      /*  for (int i = 1; (i <= current_line_offset) && (i < MAX_OFFSET); i++) {
//            gVideo[current_line_offset+80 + i].color = 10;
//            gVideo[current_line_offset+80 + i].c = CLIBuffer[i];
//
//        }
//        CursorPosition(current_line_offset + 80 + current_line_offset + 1);
//        */
//    }
//    else if (C == KEY_BACKSPACE)
//    {
//        
//       
//       /* if (SCREEN_OFFSET-1 > last_enter_offset)
//        {*/
//            current_line_offset--;
//            gVideo[SCREEN_OFFSET].color = 10;
//            C = ' ';
//            gVideo[SCREEN_OFFSET].c = C;
//            CLIBuffer[current_line_offset] = C;
//            CursorPosition(SCREEN_OFFSET);
//        //}
//       
//        
//    }
//   
//    else if (is_ext == 1 && C == KEY_UP) {
//        /*current_line_offset*/
//        current_row--;
//        CursorPosition(SCREEN_OFFSET);
//    }
//    else if (is_ext == 1 && C == KEY_LEFT) {
//        current_line_offset--;
//        //current_row
//        CursorPosition(SCREEN_OFFSET);
//    }
//    else if (is_ext == 1 && C == KEY_RIGHT) {
//        current_line_offset++;
//        //current_row
//        CursorPosition(SCREEN_OFFSET);
//    }
//    else if (is_ext == 1 && C == KEY_DOWN) {
//        //current_line_offset
//        current_row++;
//        CursorPosition(SCREEN_OFFSET);
//    }
//   
//    
//    
//}


void PutString(char *Buffer)
{
    //size_t len = 0;
    //while (len < MAX_OFFSET && Buffer[len]!=0)
    //{
    //    len++;
    //}

    //size_t i;
    //for (i = 0; (i < len) && (i < MAX_OFFSET); i++)
    //{
    //    if (!is_format_char(Buffer[i])) {
    //        gVideo[current_line_offset + i].color = 10;
    //        gVideo[current_line_offset + i].c = Buffer[i];
    //    }
    //    else if (Buffer[i] == '\n') {
    //        //CursorPosition(currentRow * MAX_COLUMNS + currentColumn);
    //        column
    //    }
    //   
    //}
    //CursorPosition(current_line_offset + i);
    //current_line_offset += i;
}


void ScreenDisplay(char* logBuffer, int color)
{
    int i, currentColumn = 0, currentRow = 0;

    // Ensure the logBuffer is not NULL
    if (logBuffer == NULL)
        return;

    // Write the logBuffer to video memory
    for (i = 0; logBuffer[i] != '\0' && i < MAX_OFFSET; i++)
    {
        if (logBuffer[i] == '\n')
        {
            // Move to the next line
            currentRow++;
            currentColumn = 0; // Reset column to the start of the line

            // If we've reached the maximum number of rows, clear the screen or scroll
            if (currentRow >= MAX_LINES)
            {
                ClearScreen(); // Clear the screen, or implement scrolling logic if desired
                currentRow = 0; // Reset to the first row
            }
        }
        else
        {
            // Set the character and its color in video memory
            int pos = currentRow * MAX_COLUMNS + currentColumn; // Calculate the position in video memory

            // Ensure we don't write beyond the screen buffer
            if (pos < MAX_OFFSET)
            {
                gVideo[pos].c = logBuffer[i];
                gVideo[pos].color = color;

                currentColumn++;
                // If we reach the end of a row, move to the next line
                if (currentColumn >= MAX_COLUMNS)
                {
                    currentColumn = 0; // Reset column to start
                    currentRow++; // Move to the next row
                }

                // If we've reached the maximum number of rows, clear the screen or scroll
                if (currentRow >= MAX_LINES)
                {
                    ClearScreen(); // Clear the screen, or implement scrolling logic if desired
                    currentRow = 0; // Reset to the first row
                }
            }
        }
    }

    // Move cursor to the last position after writing
    CursorPosition(SCREEN_OFFSET);
    current_line_offset = currentRow * MAX_COLUMNS + currentColumn + 1;
}

