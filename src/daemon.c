#include "../include/daemon.h"
#include <ctype.h>
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

char *read_stdin_content() {
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

void copy_str_into_str(char *dest, const char *src) {
  strncpy(dest, src, strlen(src));
  dest = dest + strlen(src);
}

void copy_mnemonic_into_str(char *dest, const uint8_t opcode) {
  copy_str_into_str(dest, opcode_to_mnemonic[opcode].name);
}

void copy_reg_into_str(char *dest, const uint8_t reg) {
  copy_str_into_str(dest, strcat(" ", register_name_to_code[reg]));
}

void copy_im_into_str(char *dest, const uint32_t im) {
  char *src_str = malloc(8);
  sprintf(src_str, "%d", im);
  copy_str_into_str(dest, strcat(" ", src_str));
}

char *assembly_to_str(Instruction *instr, uint32_t machine_instr) {
  char *instr_str = malloc(23);
  char *dest = instr_str;
  char *mnemonic_str = malloc(8);
  bool is_instruction = false;
  for (size_t i = 0;
       i < sizeof(opcode_to_mnemonic) / sizeof(opcode_to_mnemonic[0]); i++) {
    if (opcode_to_mnemonic[i].value == instr->op) {
      copy_mnemonic_into_str(dest, i);
      is_instruction = true;
    }
  }
  if (is_instruction) {
    if ((ADDI <= instr->op && instr->op <= ANDI) ||
        (ADDM <= instr->op && instr->op <= ANDM)) {
      copy_reg_into_str(dest, instr->opd1);
      copy_im_into_str(dest, instr->opd2);
    } else if (ADDR <= instr->op && instr->op <= ANDR) {
      copy_reg_into_str(dest, instr->opd1);
      copy_reg_into_str(dest, instr->opd2);
    } else {
      goto else_branch;
    }
    return instr_str;
  } else {
  else_branch:
    sprintf(instr_str, "%d", machine_instr);
    return instr_str;
  }
}

void print_array(uint32_t *ar, size_t length) {
  for (size_t i = 0; i < length; i++) {
    // Format the index with leading zeros to fit 5 characters
    char index_str[6]; // 5 characters + null terminator
    snprintf(index_str, sizeof(index_str), "%05zu", i);

    // Get the assembly representation of the current value
    const char *assembly_str =
        assembly_to_str(machine_to_assembly(ar[i]), ar[i]);

    // Print the formatted index and the assembly string
    printf("%s: %s\n", index_str, assembly_str);
  }
}

void cont(void) {}
