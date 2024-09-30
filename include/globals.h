#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>
#include <stdbool.h>

extern uint32_t sram_size;
extern uint16_t page_size;
extern uint32_t hdd_size;
extern bool daemon_mode;
extern uint8_t radius;

extern char *peripherals_dir;
extern char *eprom_prgrm_path;
extern const char *sram_prgrm_path;

extern uint64_t eprom_watchpoint;
extern uint64_t sram_watchpoint;
extern uint64_t hdd_watchpoint;

#define NUM_REGISTERS 8
#define NUM_UART_ADDRESSES 8

extern uint32_t num_instrs_prgrm;
extern uint32_t num_instrs_start_prgrm;

#endif // GLOBALS_H
