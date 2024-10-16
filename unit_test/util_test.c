#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

void test_extract_line() {
  const char *text = "Line 1\nLine 2\nLine 3\nLine 4\nLine 5\nLine 6\nLine 7\n";
  const char *ptr = text + 15; // Pointer to 'i' in "Line 3"

  char *result = extract_line(ptr, text);
  assert(strcmp(result, "Line 3") == 0);
  free(result);
}

int main() {
  test_extract_line();

  return 0;
}
