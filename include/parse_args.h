#include <stdbool.h>
#include <stdint.h>

#ifndef PARSE_ARGS_H
#define PARSE_ARGS_H

extern uint32_t sram_size;
extern uint16_t page_size;
extern uint32_t hdd_size;
extern bool daemon_mode;
extern bool test_mode;
extern uint8_t radius;
extern uint8_t max_waiting_instrs;
extern bool verbose;

extern char *peripherals_dir;
extern char *eprom_prgrm_path;
extern char *sram_prgrm_path;
extern char *isrs_prgrm_path;

void parse_args(uint8_t argc, char *argv[]);
void print_args() ;

#endif // PARSE_ARGS_H
