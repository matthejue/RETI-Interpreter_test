#include "../include/assemble.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void test_assembly_to_machine() {
  String_Instruction *str_instr = malloc(sizeof(String_Instruction));
  strcpy(str_instr->op, "STOREIN");
  strcpy(str_instr->opd1, "DS");
  strcpy(str_instr->opd2, "ACC");
  strcpy(str_instr->opd3, "-2097152");
  assert(assembly_to_machine(str_instr) == 0b10010111111000000000000000000000);
}

void test_machine_to_assembly() { 
  Instruction *instr = machine_to_assembly(0b10010111110111111111111111111111);
  assert(instr->op == 0b1001000);
  assert(instr->opd1 == 0b111);
  assert(instr->opd2 == 0b011);
  assert(instr->opd3 == 2097151);
}

void test_machine_to_assembly_negative() { 
  Instruction *instr = machine_to_assembly(0b10010111111000000000000000000000);
  assert(instr->op == 0b1001000);
  assert(instr->opd1 == 0b111);
  assert(instr->opd2 == 0b011);
  assert(instr->opd3 == -2097152);
}

int main() {
  test_assembly_to_machine();
  test_machine_to_assembly();
  test_machine_to_assembly_negative();
  return 0;
}
