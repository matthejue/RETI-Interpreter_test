#include "../include/parse.h"
#include "../include/interpret.h"
#include "../include/reti.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

String_Instruction *parse_instruction(const char *instr_str) {
  String_Instruction *instr = malloc(sizeof(String_Instruction));
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

void parse_and_load_program(const char *program) {
  int count;
  char **instr_strings = tokenize(program, "\n;", &count);

  for (uint32_t i = 0; i < count; i++) {
    String_Instruction *string_instr = parse_instruction(instr_strings[i]);
    unsigned int machine_instr = assembly_to_machine(string_instr);
    write_file(sram, i, machine_instr);

    free(instr_strings[i]);
  }
  free(instr_strings);
}
