#include "../include/error.h"
#include "../include/assemble.h"
#include "../include/utils.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Error_Context error_context;

void display_error_message(char *error_type, char *error_message,
                           char *to_insert) {
  printf("%s:", error_context.filename);
  printf("%d: ",
         count_lines(error_context.code_current, error_context.code_begin));
  printf("%s: ", error_type);
  char *error_message_inserted =
      malloc(strlen(error_message) + strlen(to_insert) + 1);
  sprintf(error_message_inserted, error_message, to_insert);
  printf("%s\n", error_message_inserted);
  printf("%s\n",
         extract_line(error_context.code_current, error_context.code_begin));
}

void check_opd(OperandType opd_expected, char *opd) {
  switch (opd_expected) {
  case REG:
    if (!isalpha(opd[0])) {
      display_error_message("SyntaxError",
                            "Invalid syntax for instruction at \"%s\"", opd);
      exit(EXIT_FAILURE);
    }
    break;
  case IM:
    if (!(isdigit(opd[0]) || opd[0] == '-')) {
      display_error_message("SyntaxError",
                            "Invalid syntax for instruction at \"%s\"", opd);
      exit(EXIT_FAILURE);
    }
    break;
  case EMPTY:
    if (!(strcmp(opd, "") == 0)) {
      display_error_message("SyntaxError",
                            "Invalid syntax for instruction at \"%s\"", opd);
      exit(EXIT_FAILURE);
    }
    break;
  default:
    perror("Error: Invalid operand type");
    exit(EXIT_FAILURE);
  }
}

void check_instr(uint8_t op, String_Instruction *str_instr) {
  if ((ADDI <= op && op <= ANDI) || (ADDM <= op && op <= ANDM)) {
    check_opd(REG, str_instr->opd1);
    check_opd(IM, str_instr->opd2);
    check_opd(EMPTY, str_instr->opd3);
  } else if (ADDR <= op && op <= ANDR) {
    check_opd(REG, str_instr->opd1);
    check_opd(REG, str_instr->opd2);
    check_opd(EMPTY, str_instr->opd3);
  } else if (op == LOAD || op == LOADI) {
    check_opd(REG, str_instr->opd1);
    check_opd(IM, str_instr->opd2);
    check_opd(EMPTY, str_instr->opd3);
  } else if (op == LOADIN) {
    check_opd(REG, str_instr->opd1);
    check_opd(REG, str_instr->opd2);
    check_opd(IM, str_instr->opd3);
  } else if (op == STORE) {
    check_opd(REG, str_instr->opd1);
    check_opd(IM, str_instr->opd2);
    check_opd(EMPTY, str_instr->opd3);
  } else if (op == STOREIN) {
    check_opd(REG, str_instr->opd1);
    check_opd(REG, str_instr->opd2);
    check_opd(IM, str_instr->opd3);
  } else if (op == MOVE) {
    check_opd(REG, str_instr->opd1);
    check_opd(REG, str_instr->opd2);
    check_opd(EMPTY, str_instr->opd3);
  } else if ((JUMPGT <= op && op <= JUMP) || op == INT) {
    check_opd(IM, str_instr->opd1);
    check_opd(EMPTY, str_instr->opd2);
    check_opd(EMPTY, str_instr->opd3);
  } else if (op == NOP || op == RTI) {
    check_opd(EMPTY, str_instr->opd1);
    check_opd(EMPTY, str_instr->opd2);
    check_opd(EMPTY, str_instr->opd3);
  } else if (op == IVTE) {
    check_opd(IM, str_instr->opd1);
    check_opd(EMPTY, str_instr->opd2);
    check_opd(EMPTY, str_instr->opd3);
  } else {
    perror("Error: Invalid opcode");
    exit(EXIT_FAILURE);
  }
}
