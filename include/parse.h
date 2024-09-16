#ifndef PARSE_H
#define PARSE_H
#include "../include/assemble.h"

// Function declarations
Unique_Mnemonic parse_operation(const char *op_str);
Instruction *parse_instruction(const char *instr_str);
void parse_program(const char *program);
char *trim_whitespace(char *str);
char **tokenize(const char *input, const char *delimiters, int *count);

#endif
