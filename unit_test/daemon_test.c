#include "../include/assemble.h"
#include "../include/daemon.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void test_read_stdin() {
  FILE *input_file = fopen("input.txt", "w");
  if (input_file == NULL) {
    perror("Failed to create input file");
    exit(EXIT_FAILURE);
  }
  fprintf(input_file, "Simulated user input\n");
  fclose(input_file);

  freopen("input.txt", "r", stdin);

  char *input = read_stdin();
  assert(strcmp(input, "Simulated user input\n") == 0);

  freopen("/dev/tty", "r", stdin);

  if (remove("input.txt") != 0) {
    perror("Failed to remove input file");
    exit(EXIT_FAILURE);
  }
}

void test_assembly_to_str() {
  Instruction instr = {.op = 0b0010011, .opd1 = 0b011, .opd2 = 0b101010};
  char *instr_str = assembly_to_str(&instr);
  assert(strcmp(instr_str, "DIV ACC 42") == 0);
}

void test_assembly_to_str_negative() {
  Instruction instr = {
      .op = 0b1001000, .opd1 = 0b011, .opd2 = 0b010, .opd3 = -2097152};
  char *instr_str = assembly_to_str(&instr);
  assert(strcmp(instr_str, "STOREIN ACC IN2 -2097152") == 0);
}

void test_mem_content_to_str() {
  uint32_t mem_content = 42;
  char *instr_str = mem_content_to_str(mem_content);
  assert(strcmp(instr_str, "42") == 0);
}

void test_mem_content_to_str_negative() {
  uint32_t mem_content = -42;
  char *instr_str = mem_content_to_str(mem_content);
  assert(strcmp(instr_str, "-42") == 0);
}

void test_split_string() {
  const char *str = "This is a test string";
  uint8_t count;
  char **words = split_string(str, &count);

  char *expected_words[] = {"This", "is", "a", "test", "string"};

  for (int i = 0; i < count; i++) {
    assert(strcmp(words[i], expected_words[i]) == 0);
    assert(count == 5);
    free(words[i]); // Free each word
  }
  free(words); // Free the array of words
}


int main() {
  test_split_string();
  test_assembly_to_str();
  test_assembly_to_str_negative();
  test_mem_content_to_str();
  test_mem_content_to_str_negative();
  test_read_stdin();

  return 0;
}
