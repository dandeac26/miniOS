#include "screen.h"

static PSCREEN gVideo = (PSCREEN)(0x000B8000);
static char CLIBuffer[82];
static char VideoMemoryBuffer[MAX_OFFSET];

static char NORMAL_VideoBuffer[MAX_OFFSET];

#define SCREEN_OFFSET (current_row * MAX_COLUMNS + current_line_offset[current_row])

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
		gVideo[i].color = text_color;
		gVideo[i].c = boot[i];
	}
    CursorPosition(i);
}

void ClearScreen()
{
    int i;

    for (i = 0; i < MAX_OFFSET; i++)
    {
        gVideo[i].color = text_color;
        gVideo[i].c = ' ';
        if(ConsoleMode == NORMAL_MODE)    NORMAL_VideoBuffer[i] = ' ';
        if(ConsoleMode == EDIT_MODE)    VideoMemoryBuffer[i] = ' ';
    }

    for (int i = 0; i < MAX_LINES; i++)
        current_line_offset[i] = 0;

    current_row = 0;
    CursorPosition(SCREEN_OFFSET);
    CursorMove(0, 0);
    for (int i = 0; i < MAX_LINES; i++)
        line_size[i] = 0;
    for (int i = 0; i < MAX_LINES; i++)
        new_line[i] = false;
}

//void ClearScreenMode(CONSOLE_MODE mode)
//{
//    if (mode == NORMAL_MODE) {
//        int i;
//
//        for (i = 0; i < MAX_OFFSET; i++)
//        {
//            gVideo[i].color = text_color;
//            gVideo[i].c = ' ';
//            VideoMemoryBuffer[i] = ' ';
//        }
//        for(int i = 0 ; i<MAX_LINES; i++)
//            current_line_offset[i] = 0;
//        current_row = 0;
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
//            VideoMemoryBuffer[i] = ' ';
//        }
//        gVideo[i].color = text_color;
//        for (int i = 0; i < MAX_LINES; i++)
//            current_line_offset[i] = 0;
//        current_row = 0;
//
//        for (int i = 0; i < MAX_LINES; i++)
//            new_line[i] = 0;
//        CursorPosition(SCREEN_OFFSET);
//        CursorMove(0, 0);
//    }
//}

void EnterMode(CONSOLE_MODE mode)
{

    ConsoleMode = mode;

    if (mode == EDIT_MODE) {
        text_color = CYAN_COLOR;
        //CClearScreen(NULL, NULL, NULL, NULL, NULL, NULL);

        int row = 3, col = 5;

        //RestoreScreen(VideoMemoryBuffer, NULL, &row, &current_line_offset, &line_size, &new_line);

        for (int i = 0; i < MAX_OFFSET; i++)
        {
            gVideo[i].color = text_color;
            gVideo[i].c = VideoMemoryBuffer[i];
        }

       // check all buffered text and map new line and others
        current_row = row;        

        CursorPosition(SCREEN_OFFSET);
    }
    else 
    {
        text_color = 10;
        for (int i = 0; i < MAX_OFFSET; i++)
        {
            gVideo[i].color = text_color;
            gVideo[i].c = NORMAL_VideoBuffer[i];
        }
        current_row = 5;
        CursorPosition(SCREEN_OFFSET);
        //CClearScreen(VideoMemoryBuffer, MAX_OFFSET, current_row, &current_line_offset, line_size, new_line);
    }
}

void PutChar(KEYCODE C, int is_ext)
{
    // Handling regular keys when is_ext == 0
    if (is_ext == false) {

        if (C == ENTER_KEY || C == ENTER_KEY2) {
            
            current_row++;
            new_line[current_row] = true;
            CursorPosition(SCREEN_OFFSET);
            
            if (ConsoleMode == NORMAL_MODE) {
                CLIBuffer[line_size[current_row-1]] = '\0';
                ParseCommand(CLIBuffer, line_size[current_row-1]);

                line_size[current_row] = 0;
            }
        }
        else if (C == BACKSPACE_KEY && current_line_offset[current_row] > 0) {
            current_line_offset[current_row]--;
            
            // shift left
            for (int i = current_line_offset[current_row]; i < line_size[current_row]; i++) {
                if (i == line_size[current_row] - 1)
                {
                    gVideo[current_row * MAX_COLUMNS + i].color = text_color;
                    gVideo[current_row * MAX_COLUMNS + i].c = ' ';
                    if(ConsoleMode == EDIT_MODE)    VideoMemoryBuffer[current_row * MAX_COLUMNS + i] = ' ';
                    else if(ConsoleMode == NORMAL_MODE)  NORMAL_VideoBuffer[current_row * MAX_COLUMNS + i] = ' ';
                }
                else 
                {
                    gVideo[current_row * MAX_COLUMNS + i].color = text_color;
                    gVideo[current_row * MAX_COLUMNS + i].c = gVideo[current_row * MAX_COLUMNS + i + 1].c;
                    if (ConsoleMode == EDIT_MODE)   VideoMemoryBuffer[current_row * MAX_COLUMNS + i] = VideoMemoryBuffer[current_row * MAX_COLUMNS + i + 1];
                    else if (ConsoleMode == NORMAL_MODE)  NORMAL_VideoBuffer[current_row * MAX_COLUMNS + i] = NORMAL_VideoBuffer[current_row * MAX_COLUMNS + i + 1];
                    
                }
               
            }

            
            CLIBuffer[line_size[current_row]] = '\0';
            CursorPosition(SCREEN_OFFSET);
            line_size[current_row]--;
            
        }
        else if (C == KEY_ESCAPE && ConsoleMode == EDIT_MODE) {
            EnterMode(NORMAL_MODE);
        }
        else {
            gVideo[SCREEN_OFFSET].color = text_color;

            if (is_value(C) && line_size[current_row] < MAX_COLUMNS)
            {
               // if(line_size[current_row]==-1) line_size[current_row]++;

                line_size[current_row]++;
                
                // shift right
                for (int i = line_size[current_row] - 1; i > current_line_offset[current_row]; i--) {
                    gVideo[current_row * MAX_COLUMNS + i].c = gVideo[current_row * MAX_COLUMNS + i - 1].c;
                    
                    if (ConsoleMode == EDIT_MODE)  VideoMemoryBuffer[current_row * MAX_COLUMNS + i] = VideoMemoryBuffer[current_row * MAX_COLUMNS + i - 1];
                    else if (ConsoleMode == NORMAL_MODE) NORMAL_VideoBuffer[current_row * MAX_COLUMNS + i] = NORMAL_VideoBuffer[current_row * MAX_COLUMNS + i - 1];
                    
                }
                gVideo[SCREEN_OFFSET].c = (char)C;
                if (ConsoleMode == EDIT_MODE)           VideoMemoryBuffer[SCREEN_OFFSET] = (char)C;
                else if (ConsoleMode == NORMAL_MODE)    NORMAL_VideoBuffer[SCREEN_OFFSET] = (char)C;
                CLIBuffer[current_line_offset[current_row]++] = (char)C;
            }

            if (current_line_offset[current_row] >= MAX_COLUMNS) {
                current_row++;
                new_line[current_row] = true;
                current_line_offset[current_row] = 0;
                if(ConsoleMode == NORMAL_MODE)  line_size[current_row] = 0;
                
            }

            if (current_row >= MAX_LINES) {
                ClearScreen();
                current_row = 0;
                new_line[0] = true;
                line_size[current_row] = 0;
                current_line_offset[current_row] = 0;
            }

            CursorPosition(SCREEN_OFFSET);
        }
    }

    else if (is_ext == true) {
        // Handling arrow keys
        if (C == KEY_DELETE && current_line_offset[current_row] < line_size[current_row]) {
            

            // shift left
            for (int i = current_line_offset[current_row]; i < line_size[current_row]; i++) {
                if (i == line_size[current_row] - 1)
                {
                    gVideo[current_row * MAX_COLUMNS + i].color = text_color;
                    gVideo[current_row * MAX_COLUMNS + i].c = ' ';
                   

                    if (ConsoleMode == EDIT_MODE)           VideoMemoryBuffer[current_row * MAX_COLUMNS + i] = ' ';
                    else if (ConsoleMode == NORMAL_MODE)    NORMAL_VideoBuffer[current_row * MAX_COLUMNS + i] = ' ';
                }
                else
                {
                    gVideo[current_row * MAX_COLUMNS + i].color = text_color;
                    gVideo[current_row * MAX_COLUMNS + i].c = gVideo[current_row * MAX_COLUMNS + i + 1].c;
                    if (ConsoleMode == EDIT_MODE)  VideoMemoryBuffer[current_row * MAX_COLUMNS + i] = VideoMemoryBuffer[current_row * MAX_COLUMNS + i + 1];
                    else if (ConsoleMode == NORMAL_MODE)  NORMAL_VideoBuffer[current_row * MAX_COLUMNS + i] = NORMAL_VideoBuffer[current_row * MAX_COLUMNS + i + 1];
                }

            }


            CLIBuffer[line_size[current_row]] = '\0';//here was current offeset
            CursorPosition(SCREEN_OFFSET);
            line_size[current_row]--;
        }
        else if (C == KEY_UP && current_row > 0 && ConsoleMode == EDIT_MODE) {

            current_row--;
            if (line_size[current_row] > current_line_offset[current_row + 1])
                current_line_offset[current_row] = current_line_offset[current_row + 1];
            else if(new_line[current_row] == false)
                current_line_offset[current_row] = line_size[current_row];
            //if (current_line_offset[current_row] < 0) current_line_offset[current_row] = 0;

        }
        else if (C == KEY_DOWN && current_row < MAX_LINES - 1 && ConsoleMode == EDIT_MODE) {
            if (new_line[current_row + 1] == true) {
                current_row++;

                if (line_size[current_row] > current_line_offset[current_row - 1])
                    current_line_offset[current_row] = current_line_offset[current_row - 1];
            }


        }
        else if (C == KEY_LEFT && current_line_offset[current_row] > 0) {
            current_line_offset[current_row]--;
        }
        else if (C == KEY_RIGHT && current_line_offset[current_row] < MAX_COLUMNS - 1) { // end of console
            if (current_line_offset[current_row] < line_size[current_row]) {
                current_line_offset[current_row]++;
            }
            else if(current_row < MAX_LINES - 1 && ConsoleMode == EDIT_MODE)
            {
                if (new_line[current_row + 1] == true) {
                    current_row++;

                    current_line_offset[current_row] = 0;
                }

            }
            
        }
        CursorPosition(SCREEN_OFFSET);
    }
}

void GetScreenState(void* VideoBuffer, DWORD BufferSize, int CursorPosition) 
{
    VideoBuffer = VideoMemoryBuffer;
    BufferSize = MAX_OFFSET;
    CursorPosition = SCREEN_OFFSET;
}


void PutString(char *Buffer)
{

}


void ScreenDisplay(char* logBuffer, int color)
{
    int i, currentColumn = current_line_offset[current_row], currentRow = current_row;

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
    current_row++;
    //current_line_offset[current_row] = 0;
    CursorPosition(SCREEN_OFFSET);
}

