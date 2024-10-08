#ifndef DAEMON_H
#define DAEMON_H

#include "../include/assemble.h"
#include <stdio.h>

typedef struct {
  Unique_Opcode value;
  const char *name;
} Mnemonic_to_String;

char *read_stdin();
void process_and_print_array(uint32_t *array, size_t length);
char *assembly_to_str(Instruction *instr);
char *mem_value_to_str(uint32_t mem_content, bool is_unsigned);
void print_mem_content_with_idx(uint64_t idx, uint32_t mem_content,
                                bool are_unsigned, bool are_instrs);
void print_reg_content_with_reg(uint8_t idx, uint32_t mem_content);
void print_array_with_idcs(uint32_t *ar, uint8_t length, bool are_regs,
                           bool are_instrs);
void print_file_idcs(FILE *file, uint64_t start, uint64_t end,
                     bool are_unsigned, bool are_instrs);
char **split_string(const char *str, uint8_t *count);
void cont(void);

#endif // DAEMON_H
