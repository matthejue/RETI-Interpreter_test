#include "../include/parse_instrs.h"
#include "string.h"
#include "assert.h"

void test_parse_instr() {
  String_Instruction *str_instr = parse_instr("STOREIN IN2 ACC -2097152");
  assert(strcmp(str_instr->op, "STOREIN") == 0);
  assert(strcmp(str_instr->opd1, "IN2") == 0);
  assert(strcmp(str_instr->opd2, "ACC") == 0);
  assert(strcmp(str_instr->opd3, "-2097152") == 0);
}

void test_tokenize() {
  uint32_t count;
  char **tokens = tokenize("LOADI ACC 42  ;\n    STOREIN IN2 ACC -2097152   ;", ";\n", &count);
  assert(count == 2);
  assert(strcmp(tokens[0], "LOADI ACC 42") == 0);
  assert(strcmp(tokens[1], "STOREIN IN2 ACC -2097152") == 0);
}

int main(int argc, char *argv[]) { 
  test_parse_instr();
  test_tokenize();
  return 0; 
}
