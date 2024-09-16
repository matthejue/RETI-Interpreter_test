#include "../include/parse.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to identify the operation
// Operation parse_operation(const char *op_str) {
//   if (strcmp(op_str, "MOV") == 0)
//     return MOV;
//   if (strcmp(op_str, "ADD") == 0)
//     return ADD;
//   if (strcmp(op_str, "SUB") == 0)
//     return SUB;
//   return UNKNOWN;
// }

// Function to parse a single instruction string
Instruction *parse_instruction(const char *instr_str) {
  Instruction *instr = malloc(sizeof(Instruction));
  char op[8] = {'\0'};
  char opd1[23] = {'\0'};
  char opd2[23] = {'\0'};
  char opd3[23] = {'\0'};

  // Parse operation and operands
  sscanf(instr_str, "%s %s %s %s", op, opd1, opd2, opd3);

  // Identify operation
  strcpy(instr->op, op);
  strcpy(instr->opd1, opd1);
  strcpy(instr->opd2, opd2);

  // check if if operation3 is the empty string and then copy
  if (strcmp(opd3, "") != 0) {
    strcpy(instr->opd3, opd3);
  }

  return instr;
}

// Function to process the instruction (for now, just print it)
// void process_instruction(Instruction instr) {
//   switch (instr.op) {
//   case MOV:
//     printf("MOV %s %s\n", instr.operand1, instr.operand2);
//     break;
//   case ADD:
//     printf("ADD %s %s\n", instr.operand1, instr.operand2);
//     break;
//   case SUB:
//     printf("SUB %s %s\n", instr.operand1, instr.operand2);
//     break;
//   default:
//     printf("Unknown instruction\n");
//   }
// }

// // Function to split the input by semicolons and parse each instruction
// void parse_program(const char *program) {
//     char *program_copy = strdup(program);  // Make a mutable copy of the
//     program char *instr_str = strtok(program_copy, ";");
//
//     while (instr_str != NULL || instr_str == "") {
//         // Parse and process each instruction
//         Instruction instr = parse_instruction(instr_str);
//         process_instruction(instr);
//
//         // Get the next instruction
//         instr_str = strtok(NULL, ";");
//     }
//
//     free(program_copy);
// }

char *trim_whitespace(char *str) {
  char *end;

  // Trim leading space
  while (isspace((unsigned char)*str))
    str++;

  if (*str == 0) // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while (end > str && isspace((unsigned char)*end))
    end--;

  // Write new null terminator
  *(end + 1) = '\0';

  return str;
}

char **tokenize(const char *input, const char *delimiters, int *count) {
  char *input_copy = strdup(input);
  char *token;
  char **tokens = NULL;
  int tokens_count = 0;

  token = strtok(input_copy, delimiters);
  while (token != NULL) {
    tokens = realloc(tokens, sizeof(char *) * (tokens_count + 1));
    char *trimmed_token = strdup(trim_whitespace(token));
    if (strlen(trimmed_token) > 0) {
      tokens[tokens_count++] = trimmed_token;
    }
    token = strtok(NULL, delimiters);
  }

  free(input_copy);
  *count = tokens_count;
  return tokens;
}

void parse_program(const char *program) {
  int count;
  char **instruction_strings = tokenize(program, "\n;", &count);

  for (int i = 0; i < count; i++) {
    Instruction *instruction = parse_instruction(instruction_strings[i]);
    unsigned int machine_instr = assembly_to_machine(instruction);
    free(instruction_strings[i]);
  }
  free(instruction_strings);
}
