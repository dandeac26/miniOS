#include "main.h"
#include "screen.h"
#include "logging.h"
#include "string.h"
#include "PIC.h"
#include "IO.h"
#include "ata_commands.h"


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

    Log(logBuffer); // log through serial
    ScreenDisplay(logBuffer, 10);
}

void InterruptExamples() {
    __magic();

    //__cause_div0(); //0
    __cause_pageFault(); // 14
    //__cause_int3(); // 3

    __magic();
    __haltt();
    __haltt();

    LogSerialAndScreen("Hello from main after exception");
}

void initPIT() {
    __int16 divisor = (__int16)(PIT_FREQ / 100);

    // Send the command byte
    __outbyte(PIT_CMD, 0x36);  // 0x36: Mode 3 (square wave), channel 0, lobyte/hibyte access

    // Send the frequency divisor (low byte first, then high byte)
    __outbyte(PIT_CHANNEL0, divisor & 0xFF);
    __outbyte(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}

void KernelMain()
{

    __enableSSE();  // only for demo; in the future will be called from __init.asm

    InitScreen();

    ClearScreen();

    InitLogging();

    Log("Logging initialized!");

    //HelloBoot();

    ClearScreen();


 /*   LogSerialAndScreen("Hello from main");
    __magic();*/

    
   
    //InterruptExamples(); // if this ran will halt
    //__magic();
    //ClearScreen();
    
    pic_disable(); // Initially disable all interrupts
    PIC_remap(0x20, 0x28); // setup PIC


    initPIT();  // Timer programming

    IRQ_clear_mask(0);       // Enable PIT (IRQ0)



    /*__outbyte(0x64, 0xAE);
    __outbyte(0x64, 0x60);*/
    IRQ_clear_mask(1);       // Enable KB


    //DetectATADevices();
    //__magic();

    while (1) {
        
    }
    // Keyboard programming - DONE

    // Implement a simple console - Done

    // TODO!!! read disk sectors using PIO mode ATA

    // TODO!!! Memory management: virtual, physical and heap memory allocators
}
