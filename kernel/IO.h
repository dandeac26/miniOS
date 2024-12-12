#ifndef _IO_H_
#define _IO_H_

#include "main.h"

#define KB_CMD 0x64
#define KB_STATUS 0x64  
#define KB_DATA 0x60


unsigned read_pit_count(void);

void set_pit_count(unsigned count);



#endif _IO_H_
