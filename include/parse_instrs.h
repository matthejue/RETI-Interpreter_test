#ifndef PARSE_H
#define PARSE_H
#include "../include/assemble.h"

// Function declarations
String_Instruction *parse_instr(const char *instr_str);
char *trim_whitespace(char *str);
char **tokenize(const char *input, const char *delimiters, uint32_t *count) ;
void parse_and_load_program(const char *program);

#endif
