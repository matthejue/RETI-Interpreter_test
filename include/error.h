#include <stdint.h>

#ifndef ERROR_H
#define ERROR_H

struct Error_Context {
  const char *filename;
  uint32_t line_nr;
  const char *error_code;
};

extern struct Error_Context error_context;


void display_error_message(char *error_type, char *error_message, char *error_code) ;

#endif // ERROR_H
