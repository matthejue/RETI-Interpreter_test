#ifndef DAEMON_H
#define DAEMON_H

#include "../include/assemble.h"
#include <stdio.h>

typedef struct {
  Unique_Opcode value;
  char *name;
} Mnemonic_to_String;

void cont(void);
char *read_stdin_content();
void process_and_print_array(uint32_t *array, size_t length);
char *assembly_to_str(Instruction *instr, uint32_t machine_instr);

#endif // DAEMON_H
