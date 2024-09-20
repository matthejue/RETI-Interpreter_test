#ifndef INTERPRET_H
#define INTERPRET_H

#include "../include/assemble.h"

uint32_t mod(int32_t a, int32_t b);
void interpr_instruction(Instruction *assembly_instr);
void interpr_program();

#endif // INTERPRET_H
