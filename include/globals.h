#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>
#include <stdbool.h>

extern uint32_t ram_size;
extern uint16_t page_size;
extern uint32_t hdd_size;
extern bool daemon_mode;

#define NUM_REGISTERS 8
#define NUM_INSTRUCTIONS_START_PROGRAM 16
#define NUM_UART_ADDRESSES 8


#endif // GLOBALS_H
