#include "main.h"
#include "screen.h"
#include "logging.h"
#include "string.h"
#include "PIC.h"
#include "IO.h"

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
    //__magic();    // break into BOCHS
    
    __enableSSE();  // only for demo; in the future will be called from __init.asm

    ClearScreen();

    InitLogging();

    Log("Logging initialized!");

    HelloBoot();

    //__magic();

    ClearScreen();

    //LogSerialAndScreen("Hello from main");
   
    //InterruptExamples(); // if this ran will halt

    //__magic();
    //ClearScreen();
    
    pic_disable(); // Initially disable all interrupts
    PIC_remap(0x20, 0x28); // setup PIC

              
    IRQ_clear_mask(0);       // Enable PIT (IRQ0)

    
    initPIT();  // Timer programming

    IRQ_clear_mask(1);       // Enable KB

    //LogSerialAndScreen("PIC initialized, IRQ0 and IRQ1 unmasked");
    //__magic();
    //ClearScreen();
   
    while (1) {
       //LogSerialAndScreen("\nPIT_cnt: %u", read_pit_count());
       //__magic();
       //ClearScreen();
    }
    // TODO!!! Keyboard programming

    // TODO!!! Implement a simple console

    // TODO!!! read disk sectors using PIO mode ATA

    // TODO!!! Memory management: virtual, physical and heap memory allocators
}
