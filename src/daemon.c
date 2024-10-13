#include "../include/daemon.h"
#include "../include/assemble.h"
#include "../include/globals.h"
#include "../include/parse_args.h"
#include "../include/reti.h"
#include "../include/utils.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IS_TYPE(var, type) (_Generic((var), type: 1, default: 0))
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

Mnemonic_to_String opcode_to_mnemonic[] = {
    {ADDI, "ADDI"},     {SUBI, "SUBI"},       {MULTI, "MULTI"},
    {DIVI, "DIVI"},     {MODI, "MODI"},       {OPLUSI, "OPLUSI"},
    {ORI, "ORI"},       {ANDI, "ANDI"},       {ADDR, "ADD"},
    {SUBR, "SUB"},      {MULTR, "MULT"},      {DIVR, "DIV"},
    {MODR, "MOD"},      {OPLUSR, "OPLUS"},    {ORR, "OR"},
    {ANDR, "AND"},      {ADDM, "ADD"},        {SUBM, "SUB"},
    {MULTM, "MULT"},    {DIVM, "DIV"},        {MODM, "MOD"},
    {OPLUSM, "OPLUS"},  {ORM, "OR"},          {ANDM, "AND"},
    {LOAD, "LOAD"},     {LOADIN, "LOADIN"},   {LOADI, "LOADI"},
    {STORE, "STORE"},   {STOREIN, "STOREIN"}, {MOVE, "MOVE"},
    {JUMPGT, "JUMP>"},  {JUMPEQ, "JUMP=="},   {JUMPEQ, "JUMP="},
    {JUMPGE, "JUMP>="}, {JUMPLT, "JUMP<"},    {JUMPNE, "JUMP!="},
    {JUMPNE, "JUMP<>"}, {JUMPLE, "JUMP<="},   {JUMP, "JUMP"},
    {INT, "INT"},       {RTI, "RTI"},         {NOP, "NOP"}};

char *copy_mnemonic_into_str(char *dest, const uint8_t opcode) {
  strcat(dest, opcode_to_mnemonic[opcode].name);
  return dest + strlen(dest);
}

char *copy_reg_into_str(char *dest, const uint8_t reg) {
  strcat(dest, " ");
  dest = strcat(dest, register_name_to_code[reg]);
  return dest + strlen(dest);
}

char *copy_im_into_str(char *dest, const uint32_t im) {
  strcpy(dest, " ");
  sprintf(dest + 1, "%d", im);
  return dest + strlen(dest);
}

char *assembly_to_str(Instruction *instr) {
  char *instr_str = malloc(25); // STOREIN ACC IN2 -2097152\0
  instr_str[0] = '\0';
  char *dest = instr_str;
  for (size_t i = 0;
       i < sizeof(opcode_to_mnemonic) / sizeof(opcode_to_mnemonic[0]); i++) {
    if (opcode_to_mnemonic[i].value == instr->op) {
      dest = copy_mnemonic_into_str(dest, i);
      break;
    }
  }
  if ((ADDI <= instr->op && instr->op <= ANDI) ||
      (ADDM <= instr->op && instr->op <= ANDM)) {
    dest = copy_reg_into_str(dest, instr->opd1);
    dest = copy_im_into_str(dest, instr->opd2);
  } else if (ADDR <= instr->op && instr->op <= ANDR) {
    dest = copy_reg_into_str(dest, instr->opd1);
    dest = copy_reg_into_str(dest, instr->opd2);
  } else if (instr->op == LOAD || instr->op == STORE || instr->op == LOADI) {
    dest = copy_reg_into_str(dest, instr->opd1);
    dest = copy_im_into_str(dest, instr->opd2);
  } else if (instr->op == LOADIN || instr->op == STOREIN) {
    dest = copy_reg_into_str(dest, instr->opd1);
    dest = copy_reg_into_str(dest, instr->opd2);
    dest = copy_im_into_str(dest, instr->opd3);
  } else if (instr->op == MOVE) {
    dest = copy_reg_into_str(dest, instr->opd1);
    dest = copy_reg_into_str(dest, instr->opd2);
  } else if ((JUMPGT <= instr->op && instr->op <= JUMP) || instr->op == INT) {
    dest = copy_im_into_str(dest, instr->opd1);
  } else if (instr->op == RTI || instr->op == NOP) {
  } else {
    perror("Invalid instruction");
    exit(EXIT_FAILURE);
  }
  return instr_str;
}
char *mem_value_to_str(uint32_t mem_content, bool is_unsigned) {
  char *instr_str = malloc(12); // -2147483649
  if (is_unsigned) {
    sprintf(instr_str, "%u", mem_content);
  } else {
    sprintf(instr_str, "%d", mem_content);
  }
  return instr_str;
}

char *reg_to_mem_pntr(uint64_t idx) {
  char *active_regs = "";
  uint32_t addr = read_array(regs, PC, false);
  uint8_t mem_type = idx & 0xC0000000;
  uint8_t rel_idx = idx & 0x3FFFFFFF;
  for (size_t i = 0; i < NUM_REGISTERS; i++) {
    uint64_t reg_addr = read_array(regs, i, false);
    if (mem_type == (reg_addr & 0xC0000000) && rel_idx == (reg_addr & 0x3FFFFFFF)) {
      active_regs = register_name_to_code[i];
      break;
    }
  }
  return active_regs;
}

// TODO:: split zwischen mem content und assembly instrs
// TODO:: Unit test dafür und die ganzen idx Funktionen
void print_mem_content_with_idx(uint64_t idx, uint32_t mem_content,
                                bool are_unsigned, bool are_instrs) {
  char idx_str[6];
  snprintf(idx_str, sizeof(idx_str), "%05zu", idx);
  const char *mem_content_str;
  if (are_instrs) {
    mem_content_str = assembly_to_str(machine_to_assembly(mem_content));
  } else {
    mem_content_str = mem_value_to_str(mem_content, are_unsigned);
  }
  printf("%s: %s%s\n", idx_str, mem_content_str, reg_to_mem_pntr(idx));
}

void print_reg_content_with_reg(uint8_t idx, uint32_t mem_content) {
  char reg_str[4];
  snprintf(reg_str, sizeof(reg_str), "%3s", register_name_to_code[idx]);
  const char *mem_content_str = mem_value_to_str(mem_content, true);
  printf("%s: %s\n", reg_str, mem_content_str);
}

void print_array_with_idcs(void *ar, uint8_t length, bool are_regs,
                           bool are_instrs, bool is_uart) {
  for (size_t i = 0; i < length; i++) {
    if (are_regs) {
      print_reg_content_with_reg(i, ((uint32_t *)ar)[i]);
    } else {
      if (is_uart) {
        print_mem_content_with_idx(i, ((uint8_t *)ar)[i], false, are_instrs);
      } else {
        print_mem_content_with_idx(i, ((uint32_t *)ar)[i], false, are_instrs);
      }
    }
  }
}

void print_file_with_idcs(FILE *file, uint64_t start, uint64_t end,
                          bool are_unsigned, bool are_instrs) {
  for (size_t i = start; i <= end; i++) {
    print_mem_content_with_idx(i, read_file(file, i), are_unsigned, are_instrs);
  }
}

char **split_string(const char *str, uint8_t *count) {
  // Copy the input string to avoid modifying the original
  char *str_copy = strdup(str);
  if (str_copy == NULL) {
    perror("strdup was not successful");
    exit(EXIT_FAILURE);
  }

  // Count the number of words
  int words = 0;
  char *token = strtok(str_copy, " \t\n");
  while (token != NULL) {
    words++;
    token = strtok(NULL, " \t\n");
  }

  // Allocate memory for the array of words
  char **result = malloc((words + 1) * sizeof(void *));
  if (result == NULL) {
    perror("malloc was not successful");
    exit(EXIT_FAILURE);
  }

  // Reset the copy of the string
  strcpy(str_copy, str);

  // Split the string and store the words in the array
  int idx = 0;
  token = strtok(str_copy, " \t\n");
  while (token != NULL) {
    result[idx] = strdup(token);
    if (result[idx] == NULL) {
      perror("strdup was not successful");
      exit(EXIT_FAILURE);
    }
    idx++;
    token = strtok(NULL, " \t\n");
  }
  result[idx] = NULL;

  // Set the count of words
  *count = words;

  // Free the copy of the string
  free(str_copy);

  return result;
}

void cont(void) {
  print_array_with_idcs(regs, NUM_REGISTERS, true, false, false);
  printf("SRAM Watchpoint: %lu\n", sram_watchpoint);
  print_array_with_idcs(eprom, num_instrs_start_prgrm, false, true, false);
  print_array_with_idcs(uart, NUM_UART_ADDRESSES, false, false, true);
  print_file_with_idcs(sram, max(0, sram_watchpoint - radius),
                       min(sram_watchpoint + radius, ivt_max_idx), true, false);
  print_file_with_idcs(
      sram, max(ivt_max_idx + 1, sram_watchpoint - radius),
      min(sram_watchpoint + radius, num_instrs_isrs + num_instrs_prgrm - 1),
      false, true);
  print_file_with_idcs(
      sram, max(num_instrs_isrs + num_instrs_prgrm, sram_watchpoint - radius),
      min(sram_watchpoint + radius, SRAM_MAX_IDX), false, false);
  // print_file_idcs(hdd, max(0, hdd_view_pos - radius),
  //                 min(hdd_view_pos + radius, hdd_size-1), false);
  while (true) {
    uint8_t count;
    char buffer[26];

    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
      perror("Error: Reading input not successful\n");
    }

    char **stdin = split_string(buffer, &count);

    if (strcmp(stdin[0], "n") == 0) {
      break;
    } else if (strcmp(stdin[0], "s") == 0) {
      break;
    } else if (strcmp(stdin[0], "q") == 0) {
      exit(EXIT_SUCCESS);
    }
  }
}
