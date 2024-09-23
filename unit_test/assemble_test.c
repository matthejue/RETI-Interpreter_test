#include "../include/assemble.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void test_assembly_to_machine() {
  String_Instruction *str_instr = malloc(sizeof(String_Instruction));
  strcpy(str_instr->op, "STOREIN");
  strcpy(str_instr->opd1, "DS");
  strcpy(str_instr->opd2, "ACC");
  strcpy(str_instr->opd3, "−2097152");
  assert(assembly_to_machine(str_instr) == 0b10010111111000000000000000000000);
}

// void test_machine_to_assembly() { machine_to_assembly(); }

int main(int argc, char *argv[]) {
  test_assembly_to_machine();
  // test_machine_to_assembly();
  return 1;
}
