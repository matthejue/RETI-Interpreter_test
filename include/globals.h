#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>
#include <stdbool.h>

extern uint32_t sram_size;
extern uint16_t page_size;
extern uint32_t hdd_size;
extern bool daemon_mode;
extern uint8_t radius;

extern char *file_dir;

extern uint64_t eprom_watchpoint;
extern uint64_t sram_watchpoint;
extern uint64_t hdd_watchpoint;

#define NUM_REGISTERS 8
#define NUM_INSTRUCTIONS_START_PROGRAM 16
#define NUM_UART_ADDRESSES 8

extern uint32_t num_instrs_prgrm;


#endif // GLOBALS_H
