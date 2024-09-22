#include "../include/daemon.h"
#include "../include/globals.h"
#include "../include/reti.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    {JUMPNE, "JUMP<>"}, {JUMPLE, "JUMP<"},    {JUMP, "JUMP"},
    {INT, "INT"},       {RTI, "RTI"}};

char *read_stdin() {
  size_t len = 0;
  size_t read;
  char *line = NULL;
  char *content = NULL;
  while ((read = getline(&line, &len, stdin)) != -1) {
    if (content == NULL) {
      content = strdup(line);
    } else {
      content = realloc(content, strlen(content) + read + 1);
      strcat(content, line);
    }
  }
  free(line);
  return content;
}

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

char *assembly_to_str(Instruction *instr, uint32_t mem_content) {
  char *instr_str = malloc(23);
  instr_str[0] = '\0';
  char *dest = instr_str;
  bool is_instruction = false;
  for (size_t i = 0;
       i < sizeof(opcode_to_mnemonic) / sizeof(opcode_to_mnemonic[0]); i++) {
    if (opcode_to_mnemonic[i].value == instr->op) {
      dest = copy_mnemonic_into_str(dest, i);
      is_instruction = true;
      break;
    }
  }
  if (is_instruction) {
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
    }
    // else if (instr->op == RTI || instr->op == NOP)
    return instr_str;
  } else {
    sprintf(instr_str, "%d", mem_content);
    return instr_str;
  }
}

void print_assembly_instr_with_idx(uint64_t idx, uint32_t machine_instr) {
  char idx_str[6];
  snprintf(idx_str, sizeof(idx_str), "%05zu", idx);

  const char *assembly_str =
      assembly_to_str(machine_to_assembly(machine_instr), machine_instr);

  printf("%s: %s\n", idx_str, assembly_str);
}

void print_array(uint32_t *ar, uint8_t length) {
  for (size_t i = 0; i < length; i++) {
    print_assembly_instr_with_idx(i, ar[i]);
  }
}

void print_file(FILE *file, uint64_t start, uint64_t end) {
  for (size_t i = start; i <= end; i++) {
    print_assembly_instr_with_idx(i, read_file(file, i));
  }
}

char **split_string(const char *str, int *count) {
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


int32_t max(int32_t a, int32_t b) { return (a > b) ? a : b; }

int32_t min(int32_t a, int32_t b) { return (a < b) ? a : b; }

void cont(void) {
  uint64_t center_sram = 0;
  uint64_t center_hdd = 0;
  print_array(regs, NUM_REGISTERS);
  print_array(eprom, NUM_INSTRUCTIONS_START_PROGRAM);
  print_array(uart, NUM_UART_ADDRESSES);
  print_file(sram, max(0, center_sram - radius),
             min(center_sram + radius, sram_size));
  print_file(hdd, max(0, center_hdd - radius),
             min(center_hdd + radius, hdd_size));
  while (true) {
    char *stdin = read_stdin();

    if (strcmp(stdin, "next") == 0) {
      break;
    } else if (strcmp(stdin, "set") == 0) {
      break;
    }
  }
}
