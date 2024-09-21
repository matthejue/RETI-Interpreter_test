#include "../include/daemon.h"
#include "../include/stdlib.h"

int main() {
  const char *str = "This is a test string";
  int count;
  char **words = split_string(str, &count);

  for (int i = 0; i < count; i++) {
    printf("Word %d: %s\n", i + 1, words[i]);
    free(words[i]); // Free each word
  }
  free(words); // Free the array of words

  return 0;
}
