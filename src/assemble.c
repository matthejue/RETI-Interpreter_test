#include "../include/parse_instrs.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *register_name_to_code[] = {"PC", "IN1", "IN2", "ACC",
                                       "SP", "BAF", "CS",  "DS"};

String_to_Mnemonic mnemonic_to_opcode[] = {
    {"ADDI", ADDI},     {"SUBI", SUBI},     {"MULTI", MULTI},
    {"DIVI", DIVI},     {"MODI", MODI},     {"OPLUSI", OPLUSI},
    {"ORI", ORI},       {"ANDI", ANDI},     {"ADD", ADDR},
    {"SUB", SUBR},      {"MULT", MULTR},    {"DIV", DIVR},
    {"MOD", MODR},      {"OPLUS", OPLUSR},  {"OR", ORR},
    {"AND", ANDR},      {"LOAD", LOAD},     {"LOADIN", LOADIN},
    {"LOADI", LOADI},   {"STORE", STORE},   {"STOREIN", STOREIN},
    {"MOVE", MOVE},     {"JUMP>", JUMPGT},  {"JUMP==", JUMPEQ},
    {"JUMP=", JUMPEQ},  {"JUMP>=", JUMPGE}, {"JUMP<", JUMPLT},
    {"JUMP!=", JUMPNE}, {"JUMP<>", JUMPNE}, {"JUMP<", JUMPLE},
    {"JUMP", JUMP},     {"INT", INT},       {"RTI", RTI}};

uint8_t get_register_code(char *reg) {
  for (uint8_t i = 0;
       i < sizeof(register_name_to_code) / sizeof(register_name_to_code[0]);
       i++) {
    if (strcmp(reg, register_name_to_code[i]) == 0) {
      return i;
    }
  }
  return -1; // Invalid register
}

uint8_t get_mnemonic(char *mnemonic) {
  for (uint8_t i = 0;
       i < sizeof(mnemonic_to_opcode) / sizeof(mnemonic_to_opcode[0]); ++i) {
    if (strcmp(mnemonic, mnemonic_to_opcode[i].name) == 0) {
      return (uint8_t)mnemonic_to_opcode[i].value;
    }
  }
  return (uint8_t)UNKNOWN;
}

uint32_t assembly_to_machine(String_Instruction *instr) {
  uint32_t machine_instr = 0;
  uint8_t op = get_mnemonic(instr->op);

  if (ADDR <= op && op <= ANDR) {
    if (isdigit(instr->opd2[0])) {
      op += 8;
    }
  }

  uint32_t opd1, opd2, opd3;
  if (strcmp(instr->opd1, "") != 0) {
    if (isalpha(instr->opd1[0])) {
      opd1 = get_register_code(instr->opd1);
    } else {
      opd1 = atoi(instr->opd1);
    }
  } else {
    opd1 = 0;
  }

  if (strcmp(instr->opd2, "") != 0) {
    if (isalpha(instr->opd2[0])) {
      opd2 = get_register_code(instr->opd2);
    } else {
      opd2 = atoi(instr->opd2);
    }
  } else {
    opd2 = 0;
  }

  if (strcmp(instr->opd3, "") != 0) {
    if (isalpha(instr->opd3[0])) {
      opd3 = get_register_code(instr->opd3);
    } else {
      opd3 = atoi(instr->opd3) & IMMEDIATE_MASK;
    }
  } else {
    opd3 = 0;
  }

  if ((ADDI <= op && op <= ANDI) || (ADDM <= op && op <= ANDM)) {
    machine_instr = op << 25 | opd1 << 22 | opd2;
  } else if (ADDR <= op && op <= ANDR) {
    machine_instr = op << 25 | opd1 << 22 | opd2 << 19;
  } else if (op == LOAD || op == LOADI) {
    machine_instr = op << 25 | opd1 << 22 | opd2;
  } else if (op == LOADIN) {
    machine_instr = op << 25 | opd1 << 25 | opd2 << 22 | opd3;
  } else if (op == STORE) {
    machine_instr = op << 25 | opd1 << 25 | opd2;
  } else if (op == STOREIN) {
    machine_instr = op << 25 | opd1 << 22 | opd2 << 25 | opd3;
  } else if (op == MOVE) {
    machine_instr = op << 28 | opd1 << 25 | opd2 << 22;
  } else if (NOP <= op && op <= JUMP) {
    machine_instr = op << 25 | opd1;
  }

  return machine_instr;
}

uint32_t sign_extend_22_to_32(uint32_t num) {
  // Check if the number is negative by checking the 21st bit
  if (num & (1 << 21)) {
    // If negative, set the upper bits to 1
    num |= ~((1 << 22) - 1);
  } else {
    // If positive, ensure the upper bits are 0
    num &= (1 << 22) - 1;
  }
  return num;
}

Instruction *machine_to_assembly(uint32_t machine_instr) {
  Instruction *instr = malloc(sizeof(Instruction));
  uint8_t mode = machine_instr >> 30;
  if (mode == 0) {
    uint8_t compute_mode = machine_instr >> 25;
    uint8_t d = (machine_instr >> 22) & REGISTER_MASK;
    uint8_t s = (machine_instr >> 19) & REGISTER_MASK;
    uint32_t i;
    if (ADDI <= compute_mode && compute_mode <= ANDI) {
      i = sign_extend_22_to_32(machine_instr & IMMEDIATE_MASK);
    } else if (ADDM <= compute_mode && compute_mode <= ANDM) {
      i = machine_instr & IMMEDIATE_MASK;
    } else if (ADDR <= compute_mode && compute_mode <= ANDR) {
    }

    instr->op = compute_mode;

    if ((ADDI <= compute_mode && compute_mode <= ANDI) ||
        (ADDM <= compute_mode && compute_mode <= ANDM)) {
      instr->opd1 = (uint32_t)d;
      instr->opd2 = (uint32_t)i;
    } else if (ADDR <= compute_mode && compute_mode <= ANDR) {
      instr->opd1 = (uint32_t)d;
      instr->opd2 = (uint32_t)s;
    } else {
      perror("Error a instruction with this opcode doesn't exist yet");
      exit(EXIT_FAILURE);
    }
  } else if (mode == 1 || mode == 2) {
    uint8_t load_store_mode = machine_instr >> 28;
    load_store_mode = load_store_mode << 3;
    uint8_t s = (machine_instr >> 25) & REGISTER_MASK;
    uint8_t d = (machine_instr >> 22) & REGISTER_MASK;
    uint32_t i;

    if (load_store_mode == LOAD || load_store_mode == STORE ||
        load_store_mode == LOADI) {
      i = machine_instr & IMMEDIATE_MASK;
    } else if (load_store_mode == LOADIN || load_store_mode == STOREIN) {
      i = sign_extend_22_to_32(machine_instr & IMMEDIATE_MASK);
    }
    // TODO: Tobias fragen, ob es nicht mehr Sinn ergibt, wenn LOADI das i
    // signed ist

    instr->op = load_store_mode;

    switch (load_store_mode) {
    case LOAD:
      instr->opd1 = d;
      instr->opd2 = i;
      break;
    case LOADIN:
      instr->opd1 = s;
      instr->opd2 = d;
      instr->opd3 = i;
      break;
    case LOADI:
      instr->opd1 = d;
      instr->opd2 = i;
      break;
    case STORE:
      instr->opd1 = s;
      instr->opd2 = i;
      break;
    case STOREIN:
      instr->opd1 = d;
      instr->opd2 = s;
      instr->opd3 = i;
      break;
    case MOVE:
      instr->opd1 = s;
      instr->opd1 = d;
      break;
    default:
      perror("Error a instruction with this opcode doesn't exist yet");
      exit(EXIT_FAILURE);
    }
  } else { // mode == 3
    uint8_t jump_mode = machine_instr >> 25;
    uint32_t i = 0;
    if (jump_mode == INT) {
      i = machine_instr & IMMEDIATE_MASK;
    } else if (jump_mode == RTI || jump_mode == NOP) {
    } else {
      i = sign_extend_22_to_32(machine_instr & IMMEDIATE_MASK);
    }

    instr->op = jump_mode;

    if ((JUMPGT <= jump_mode && jump_mode <= JUMP) || jump_mode == INT) {
      instr->opd1 = i;
    } else if (jump_mode == RTI || jump_mode == NOP) {
    } else {
      perror("Error a instruction with this opcode doesn't exist yet");
      exit(EXIT_FAILURE);
    }
  }
  return instr;
}
