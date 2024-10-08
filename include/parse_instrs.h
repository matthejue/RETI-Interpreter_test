#ifndef PARSE_H
#define PARSE_H
#include "../include/assemble.h"
#include <stdio.h>

typedef enum { EPROM_START_PRGRM, SRAM_PRGRM, ISR_PRGRMS } Program_Type;

String_Instruction *parse_instr(const char **orignal_prgrm_pntr);
void parse_and_load_program(char *prgrm, Program_Type memory_type) ;

#endif
