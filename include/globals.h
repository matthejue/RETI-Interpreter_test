#include <stdint.h>
#include <stdbool.h>

#ifndef GLOBALS_H
#define GLOBALS_H

extern uint32_t ivt_max_idx;
extern uint32_t num_instrs_prgrm;
extern uint32_t num_instrs_start_prgrm;
extern uint32_t num_instrs_isrs;

extern bool breakpoint_encountered;

#endif // GLOBALS_H
