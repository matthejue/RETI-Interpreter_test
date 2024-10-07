#if 0

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *read_stdin_content() {
  size_t len = 0;
  size_t read;
  char *line = NULL;
  char *content = NULL;

  while ((read = getline(&line, &len, stdin)) != -1) {
    if (content == NULL) {
      content = strdup(line);
    } else {
      content = realloc(content, strlen(content) + read + 1);
      strcat(content, line);
    }
  }

  free(line);
  return content;
}

#endif
