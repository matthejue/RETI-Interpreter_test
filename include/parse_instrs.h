#ifndef PARSE_H
#define PARSE_H
#include "../include/assemble.h"
#include <stdio.h>

typedef enum { EPROM, UART, HDD, SRAM } Memory_Type;

// Function declarations
String_Instruction *parse_instr(const char **orignal_prgrm_pntr);
void parse_and_load_program(char *prgrm, Memory_Type mem_type);

#endif
