#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Error_Context {
  char *filename;
  uint32_t line_nr;
  char *error_code;
};

struct Error_Context error_context;

void display_error_message(char *error_type, char *error_message,
                           char *to_insert) {
  printf("%s:", error_context.filename);
  printf("%d: ", error_context.line_nr);
  printf("%s: ", error_type);
  char *error_message_inserted =  malloc(strlen(error_message) + strlen(to_insert) + 1);
  sprintf(error_message_inserted, error_message, to_insert);
  printf("%s\n", error_message_inserted);
  printf("%s\n", error_context.error_code);
}
