#ifndef PARSE_H
#define PARSE_H
#include "../include/assemble.h"

// Function declarations
String_Instruction *parse_instr(const char **orignal_prgrm_pntr);
void parse_and_load_program(const char *program);

#endif
