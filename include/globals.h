#include <stdint.h>
#include <stdbool.h>

#ifndef GLOBALS_H
#define GLOBALS_H
extern uint32_t ivt_max_idx;
extern uint32_t num_instrs_prgrm;
extern uint32_t num_instrs_start_prgrm;
extern uint32_t num_instrs_isrs;

#define MAX_WAITING_TIME 5

#endif // GLOBALS_H
