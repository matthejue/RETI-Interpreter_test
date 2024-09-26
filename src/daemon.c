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
    {JUMPNE, "JUMP<>"}, {JUMPLE, "JUMP<="},   {JUMP, "JUMP"},
    {INT, "INT"},       {RTI, "RTI"},         {NOP, "NOP"}};

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
char *mem_content_to_str(uint32_t mem_content) {
  char *instr_str = malloc(12); // -2147483649
  sprintf(instr_str, "%d", mem_content);
  return instr_str;
}

// TODO:: split zwischen mem content und assembly instrs
// TODO:: Unit test dafür und die ganzen idx Funktionen
void print_mem_content_with_idx(uint64_t idx, uint32_t mem_content) {
  char idx_str[6];
  snprintf(idx_str, sizeof(idx_str), "%05zu", idx);

  const char *assembly_str = mem_content_to_str(mem_content);

  printf("%s: %s\n", idx_str, assembly_str);
}

void print_array_with_idcs(uint32_t *ar, uint8_t length) {
  for (size_t i = 0; i < length; i++) {
    print_mem_content_with_idx(i, ar[i]);
  }
}

void print_file_idcs(FILE *file, uint64_t start, uint64_t end) {
  for (size_t i = start; i <= end; i++) {
    print_mem_content_with_idx(i, read_file(file, i));
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

int32_t max(int32_t a, int32_t b) { return (a > b) ? a : b; }

int32_t min(int32_t a, int32_t b) { return (a < b) ? a : b; }

void cont(void) {
  uint64_t center_sram = 0;
  uint64_t center_hdd = 0;
  print_array_with_idcs(regs, NUM_REGISTERS);
  print_array_with_idcs(eprom, NUM_INSTRUCTIONS_START_PROGRAM);
  print_array_with_idcs(uart, NUM_UART_ADDRESSES);
  print_file_idcs(sram, max(0, center_sram - radius),
                  min(center_sram + radius, sram_size));
  print_file_idcs(hdd, max(0, center_hdd - radius),
                  min(center_hdd + radius, hdd_size));
  while (true) {
    uint8_t count;
    char **stdin = split_string(read_stdin(), &count);
    // TODO: count not used

    if (strcmp(stdin[0], "next") == 0) {
      break;
    } else if (strcmp(stdin[0], "set") == 0) {
      break;
    }
  }
}
