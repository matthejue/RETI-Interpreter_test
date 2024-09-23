#include "../include/parse_instrs.h"
#include "../include/interpret.h"
#include "../include/reti.h"
#include "../include/globals.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t num_instrs_prgrm = 0;

// TODO: Unit tests für parse_instrs usw.
// TODO: Irgendwie durch untere Funktionen dafür sorgen, dass immer nur ein space zwischhen den %s ist
String_Instruction *parse_instr(const char *instr_str) {
  String_Instruction *instr = malloc(sizeof(String_Instruction));
  char op[8] = {'\0'};
  char opd1[9] = {'\0'};
  char opd2[9] = {'\0'};
  char opd3[9] = {'\0'};

  // Parse operation and operands
  sscanf(instr_str, "%s %s %s %s", op, opd1, opd2, opd3);

  // Identify operation
  strcpy(instr->op, op);
  strcpy(instr->opd1, opd1);
  strcpy(instr->opd2, opd2);

  // check if if operation3 is the empty string and then copy
  // TODO: nen check machen, ob Zahl nicht zu lang, später in ner anderen Funktion dann check, ob Zahl nicht zu groß ist
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

// TODO: Was ist mid LOADI   ACC   12 usw.?
char **tokenize(const char *input, const char *delimiters, uint32_t *count) {
  char *input_copy = strdup(input);
  char *token;
  char **tokens = NULL;
  uint32_t tokens_count = 0;

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
  char **instr_strings = tokenize(program, "\n;", &num_instrs_prgrm);

  for (uint32_t i = 0; i < num_instrs_prgrm; i++) {
    String_Instruction *string_instr = parse_instr(instr_strings[i]);
    uint32_t machine_instr = assembly_to_machine(string_instr);
    write_file(sram, i, machine_instr);

    free(instr_strings[i]);
  }
  free(instr_strings);
}
