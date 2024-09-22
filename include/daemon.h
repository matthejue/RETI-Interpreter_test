#ifndef DAEMON_H
#define DAEMON_H

#include "../include/assemble.h"
#include <stdio.h>

typedef struct {
  Unique_Opcode value;
  const char *name;
} Mnemonic_to_String;

void cont(void);
char *read_stdin();
void process_and_print_array(uint32_t *array, size_t length);
char *assembly_to_str(Instruction *instr, uint32_t machine_instr);
char **split_string(const char *str, int *count);

#endif // DAEMON_H
