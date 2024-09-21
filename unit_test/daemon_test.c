#include "../include/daemon.h"
#include <stdlib.h>

void test_split_string() {
  const char *str = "This is a test string";
  int count;
  char **words = split_string(str, &count);

  for (int i = 0; i < count; i++) {
    printf("Word %d: %s\n", i + 1, words[i]);
    free(words[i]); // Free each word
  }
  free(words); // Free the array of words
}

int main() {
  test_split_string();

  return 0;
}
