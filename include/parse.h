#ifndef PARSE_H
#define PARSE_H
#include "../include/assemble.h"

// Function declarations
String_Instruction *parse_instruction(const char *instr_str);
char *trim_whitespace(char *str);
char **tokenize(const char *input, const char *delimiters, int *count);
void parse_program(const char *program);

#endif
