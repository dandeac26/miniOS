#include "screen.h"

static PSCREEN gVideo = (PSCREEN)(0x000B8000);

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

    CursorMove(0, 0);
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
    CursorPosition(currentRow * MAX_COLUMNS + currentColumn);
}

