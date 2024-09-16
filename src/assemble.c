#include "../include/parse.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *register_name_to_code[] = {"PC", "ACC", "IN1", "IN2",
                                       "SP", "BAF", "CS",  "DS"};

String_to_Mnemonic mnemonic_to_opcode[] = {
    {"ADDI", ADDI},     {"SUBI", SUBI},     {"MULTI", MULTI},
    {"DIVI", DIVI},     {"MODI", MODI},     {"OPLUSI", OPLUSI},
    {"ORI", ORI},       {"ANDI", ANDI},     {"ADD", ADDR},
    {"SUB", SUBR},      {"MULT", MULTR},    {"DIV", DIVR},
    {"MOD", MODR},      {"OPLUS", OPLUSR},  {"OR", ORR},
    {"AND", ANDR},      {"LOAD", LOAD},     {"LOADIN", LOADIN},
    {"LOADI", LOAD},    {"STORE", STORE},   {"STOREIN", STOREIN},
    {"MOVE", MOVE},     {"JUMP>", JUMPGT},  {"JUMP==", JUMPEQ},
    {"JUMP=", JUMPEQ},  {"JUMP>=", JUMPGE}, {"JUMP<", JUMPLT},
    {"JUMP!=", JUMPNE}, {"JUMP<>", JUMPNE}, {"JUMP<", JUMPLE},
    {"JUMP", JUMP},     {"INT", INT},       {"RTI", RTI}};

int get_register_code(const char *reg) {
  for (int i = 0;
       i < sizeof(register_name_to_code) / sizeof(register_name_to_code[0]);
       i++) {
    if (strcmp(reg, register_name_to_code[i]) == 0) {
      return i;
    }
  }
  return -1; // Invalid register
}
//
// const char *get_register_name(int code) {
//   if (code >= 0 && code < NUM_REGISTERS) {
//     return register_names[code];
//   }
//   return NULL; // Invalid code
// }

Unique_Opcode get_mnemonic(const char *mnemonic) {
  for (int i = 0;
       i < sizeof(mnemonic_to_opcode) / sizeof(mnemonic_to_opcode[0]); ++i) {
    if (strcmp(mnemonic, mnemonic_to_opcode[i].name) == 0) {
      return mnemonic_to_opcode[i].value;
    }
  }
  return UNKNOWN;
}

unsigned int assembly_to_machine(String_Instruction *instr) {
  unsigned int machine_instruction = 0;
  unsigned int op = get_mnemonic(instr->op);
  if (ADDR <= op && op <= ANDR) {
    if (isdigit(instr->opd2[0])) {
      op += 8;
    }
  }

  // check if intr->op1 is a letter or number
  unsigned opd1, opd2, opd3;
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
      opd1 = atoi(instr->opd2);
    }
  } else {
    opd2 = 0;
  }
  if (strcmp(instr->opd3, "") != 0) {
    if (isalpha(instr->opd3[0])) {
      opd3 = get_register_code(instr->opd3);
    } else {
      opd3 = atoi(instr->opd3);
    }
  } else {
    opd3 = 0;
  }

  if ((ADDI <= op && op <= ANDI) || (ADDM <= op && op <= ANDM)) {
    machine_instruction = op << 25 || opd1 << 22 || opd2;
  } else if (ADDR <= op && op <= ANDR) {
    machine_instruction = op << 25 || opd1 << 22 || opd2 << 19;
  } else if (op == LOAD || op == LOADI) {
    machine_instruction = op << 28 || opd1 << 22 || opd2;
  } else if (op == LOADIN) {
    machine_instruction = op << 28 || opd1 << 25 || opd2 << 22 || opd3;
  } else if (op == STORE) {
    machine_instruction = op << 28 || opd1 << 25 || opd2;
  } else if (op == STOREIN) {
    machine_instruction = op << 28 || opd1 << 22 || opd2 << 25 || opd3;
  } else if (op == MOVE) {
    machine_instruction = op << 28 || opd1 << 25 || opd2 << 22;
  } else if (NOP <= op && op <= JUMP) {
    machine_instruction = op << 25 || opd1;
  }

  return machine_instruction;
}

Instruction *machine_to_assembly(unsigned int machine_instruction,
                                 char *assembly) {
  Instruction *instr = malloc(sizeof(Instruction));
  unsigned char mode = machine_instruction >> 30;

  if (mode == 0) {
    unsigned char compute_mode = machine_instruction >> 24;
    Register d = (machine_instruction >> 21) & REGISTER_MASK;
    Register s = (machine_instruction >> 18) & REGISTER_MASK;
    unsigned int i = machine_instruction & IMMEDIATE_MASK;

    instr->op = compute_mode;

    if ((ADDI <= compute_mode && compute_mode <= ANDI) ||
        (ADDM <= compute_mode && compute_mode <= ANDM)) {
      instr->opd1 = (Reg_or_Im)d;
      instr->opd2 = (Reg_or_Im)i;
    } else if (ADDR <= compute_mode && compute_mode <= ANDR) {
      instr->opd1 = (Reg_or_Im)d;
      instr->opd2 = (Reg_or_Im)s;
    } else {
      perror("Error a instruction with this opcode doesn't exist yet");
      exit(EXIT_FAILURE);
    }

  } else if (mode == 1 || mode == 2) {
    Unique_Opcode load_store_mode = machine_instruction >> 27;
    Register s = (machine_instruction >> 24) & REGISTER_MASK;
    Register d = (machine_instruction >> 21) & REGISTER_MASK;
    unsigned int i = machine_instruction & IMMEDIATE_MASK;

    instr->op = load_store_mode;

    switch (load_store_mode) {
    case LOAD:
      instr->opd1 = (Reg_or_Im)d;
      instr->opd2 = (Reg_or_Im)i;
      break;
    case LOADIN:
      instr->opd1 = (Reg_or_Im)s;
      instr->opd2 = (Reg_or_Im)d;
      instr->opd3 = (Reg_or_Im)i;
      break;
    case LOADI:
      instr->opd1 = (Reg_or_Im)d;
      instr->opd2 = (Reg_or_Im)i;
      break;
    case STORE:
      instr->opd1 = (Reg_or_Im)s;
      instr->opd2 = (Reg_or_Im)i;
      break;
    case STOREIN:
      instr->opd1 = (Reg_or_Im)d;
      instr->opd1 = (Reg_or_Im)s;
      instr->opd2 = (Reg_or_Im)i;
      break;
    case MOVE:
      instr->opd1 = (Reg_or_Im)s;
      instr->opd1 = (Reg_or_Im)d;
      break;
    default:
      perror("Error a instruction with this opcode doesn't exist yet");
      exit(EXIT_FAILURE);
      break;
    }
  } else { // mode == 3
    unsigned char jump_mode = machine_instruction >> 24;
    unsigned int i = machine_instruction & IMMEDIATE_MASK;

    instr->op = jump_mode;

    if (jump_mode != RTI) {
      instr->opd1 = (Reg_or_Im)i;
    }
  }
  return instr;
}

// int main() {
//     const char *assembly_instruction = "MOV ACC IN1 12345";
//     unsigned int machine_instruction =
//     assembly_to_machine(assembly_instruction);
//
//     printf("Machine instruction: %08X\n", machine_instruction);
//
//     char assembly[50];
//     machine_to_assembly(machine_instruction, assembly);
//
//     printf("Assembly instruction: %s\n", assembly);
//
//     return 0;
// }
