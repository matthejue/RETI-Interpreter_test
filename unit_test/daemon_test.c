#include "../include/daemon.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

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

void test_assembly_to_str() {
  Instruction instr = {.op = 0b0010011, .opd1 = 0b011, .opd2 = 0b101010};
  char *instr_str = assembly_to_str(&instr, 42);
  assert(strcmp(instr_str, "DIV ACC 42") == 0);
}


void test_read_stdin() {
    FILE *inputFile = fopen("input.txt", "w");
    if (inputFile == NULL) {
        perror("Failed to create input file");
        exit(EXIT_FAILURE);
    }
    fprintf(inputFile, "Simulated user input\n");
    fclose(inputFile);

    freopen("input.txt", "r", stdin);

    read_stdin();

    freopen("/dev/tty", "r", stdin);
}

int main() {
  test_split_string();
  test_assembly_to_str();
  test_read_stdin();

  return 0;
}
