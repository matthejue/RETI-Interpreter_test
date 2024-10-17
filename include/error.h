#include "../include/assemble.h"
#include <stdint.h>

#ifndef ERROR_H
#define ERROR_H

struct Error_Context {
  const char *filename;
  const char *code_current;
  const char *code_begin;
};

extern struct Error_Context error_context;

typedef enum {
  REG,
  IM, 
  EMPTY,
} OperandType;

void display_error_message(char *error_type, char *error_message, char *error_code) ;
void check_instr(uint8_t op, String_Instruction *str_instr) ;

#endif // ERROR_H
