#include "main.h"
#include "screen.h"
#include "logging.h"
#include "string.h"


#define LOG_BUF_MAX_SIZE 512

void
LogSerialAndScreen(
    char* FormatBuffer,
    ...
)
{
    char logBuffer[LOG_BUF_MAX_SIZE];
    va_list va;

    va_start(va, FormatBuffer);
    cl_vsnprintf(logBuffer, LOG_BUF_MAX_SIZE, FormatBuffer, va);

    // after call logBuffer will contain formatted buffer

    Log(logBuffer); // log through serial
    ScreenDisplay(logBuffer, 10); // display on screen - you will need to implement this part in `screen.c`
}


void KernelMain()
{
    __magic();    // break into BOCHS
    
    __enableSSE();  // only for demo; in the future will be called from __init.asm

    ClearScreen();

    InitLogging();

    Log("Logging initialized!");

    HelloBoot();

    __magic();

    ClearScreen();

    LogSerialAndScreen("Hello from main");
   
    __magic();

    //__cause_div0(); //0
    __cause_pageFault(); // 14
    //__cause_int3(); // 3

    __magic();

    LogSerialAndScreen("Hello from main after exception");
    // TODO!!! PIC programming; see http://www.osdever.net/tutorials/view/programming-the-pic
    // TODO!!! define interrupt routines and dump trap frame
    
    // TODO!!! Timer programming

    // TODO!!! Keyboard programming

    // TODO!!! Implement a simple console

    // TODO!!! read disk sectors using PIO mode ATA

    // TODO!!! Memory management: virtual, physical and heap memory allocators
}
