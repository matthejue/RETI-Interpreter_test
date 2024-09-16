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

// #define IMMEDIATE_MASK 0x3FFFFF // 22 bits for immediate value

int get_register_code(const char *reg) {
  for (int i = 0; i < NUM_REGISTERS; i++) {
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

Unique_Mnemonic get_mnemonic(const char *mnemonic) {
  for (int i = 0;
       i < sizeof(mnemonic_to_opcode) / sizeof(mnemonic_to_opcode[0]); ++i) {
    if (strcmp(mnemonic, mnemonic_to_opcode[i].name) == 0) {
      return mnemonic_to_opcode[i].value;
    }
  }
  return UNKNOWN;
}

unsigned int assembly_to_machine(Instruction *instr) {
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
  }
  if (ADDR <= op && op <= ANDR) {
    machine_instruction = op << 25 || opd1 << 22 || opd2 << 19;
  }
  if (op == LOAD || op == LOADI) {
    machine_instruction = op << 28 || opd1 << 22 || opd2;
  }
  if (op == LOADIN) {
    machine_instruction = op << 28 || opd1 << 25 || opd2 << 22 || opd3;
  }
  if (op == STORE) {
    machine_instruction = op << 28 || opd1 << 25 || opd2;
  }
  if (op == STOREIN) {
    machine_instruction = op << 28 || opd1 << 22 || opd2 << 25 || opd3;
  }
  if (op == MOVE) {
    machine_instruction = op << 28 || opd1 << 25 || opd2 << 22;
  }
  if (NOP <= op && op <= JUMP){
    machine_instruction = op << 25 || opd1;
  }

  return machine_instruction;
}

// if (num_operands >= 2) {
//   int reg_code = get_register_code(operand1);
//   if (reg_code != -1) {
//     machine_instruction |= reg_code << 24;
//   } else {
//     fprintf(stderr, "Invalid register: %s\n", operand1);
//     exit(1);
//   }
// }
//
// if (num_operands >= 3) {
//   int reg_code = get_register_code(operand2);
//   if (reg_code != -1) {
//     machine_instruction |= reg_code << 20;
//   } else {
//     unsigned int immediate = atoi(operand2) & IMMEDIATE_MASK;
//     machine_instruction |= immediate;
//   }
// }
//
// if (num_operands == 4) {
//   int reg_code = get_register_code(operand3);
//   if (reg_code != -1) {
//     machine_instruction |= reg_code << 16;
//   } else {
//     fprintf(stderr, "Invalid register: %s\n", operand3);
//     exit(1);
//   }
// }

// return machine_instruction;
// }

// void machine_to_assembly(unsigned int machine_instruction, char *assembly) {
//   unsigned int op_code = (machine_instruction >> 28) & 0xF;
//   unsigned int reg1_code = (machine_instruction >> 24) & 0xF;
//   unsigned int reg2_code = (machine_instruction >> 20) & 0xF;
//   unsigned int reg3_code = (machine_instruction >> 16) & 0xF;
//   unsigned int immediate = machine_instruction & IMMEDIATE_MASK;
//
//   // Assuming op_code 0 is "MOV"
//   if (op_code == 0) {
//     strcpy(assembly, "MOV ");
//   }
//
//   const char *reg1_name = get_register_name(reg1_code);
//   const char *reg2_name = get_register_name(reg2_code);
//   const char *reg3_name = get_register_name(reg3_code);
//
//   if (reg1_name) {
//     strcat(assembly, reg1_name);
//   }
//
//   if (reg2_name) {
//     strcat(assembly, " ");
//     strcat(assembly, reg2_name);
//   } else {
//     char immediate_str[12];
//     sprintf(immediate_str, " %u", immediate);
//     strcat(assembly, immediate_str);
//   }
//
//   if (reg3_name) {
//     strcat(assembly, " ");
//     strcat(assembly, reg3_name);
//   }
// }

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
