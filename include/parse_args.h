#ifndef PARSE_ARGS_H
#define PARSE_ARGS_H

#include <stdint.h>

char *read_file_content(const char *file_path);
void parse_args(uint8_t argc, char *argv[], char **input);

#endif // PARSE_ARGS_H
