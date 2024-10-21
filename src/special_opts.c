#include "../include/special_opts.h"
#include "../include/parse_args.h"
#include "../include/utils.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *output_file = NULL;

int8_t *extract_input_from_comment(const char *line, uint8_t *len) {
  const char *prefix;
  if (strncmp(line, "# input:", strlen("# input:")) == 0) {
    prefix = "# input:";
  } else if (strncmp(line, "#input:", strlen("#input:")) == 0) {
    prefix = "#input:";
  } else {
    return NULL;
  }

  const char *ptr = line + strlen(prefix);
  int8_t *ar = NULL;
  size_t count = 0;

  while (*ptr) {
    while (isspace((unsigned char)*ptr)) {
      ptr++;
    }

    if (*ptr == '\0') {
      break;
    }

    ar = realloc(ar, (count + 1) * sizeof(int8_t));
    if (isdigit((unsigned char)*ptr)) {
      int64_t num = strtol(ptr, (char **)&ptr, 10);
      if (num < INT8_MIN || num > INT8_MAX) {
        perror("Error: Number must be between -128 and 127");
        exit(EXIT_FAILURE);
      }
      ar[count++] = (int8_t)num;
    } else {
      ar[count++] = (int8_t)*ptr++;
    }
  }

  *len = count;
  return ar;
}

int8_t *extract_comment_metadata(const char *prgrm_path, uint8_t *len) {
  FILE *file = fopen(prgrm_path, "r");
  if (file == NULL) {
    perror("Error: Couldn't open file");
    exit(EXIT_FAILURE);
  }

  char line[256];
  int8_t *result = NULL;
  *len = 0;

  while (fgets(line, sizeof(line), file)) {
    if (line[0] == '\n') {
      continue;
    }

    if (line[0] == '#') {
      int8_t *parsed_line = extract_input_from_comment(line, len);
      if (parsed_line) {
        result = parsed_line;
      }
    } else {
      break;
    }
  }

  fclose(file);
  return result;
}

void create_output_file() {
  char *filename = strdup(sram_prgrm_path);
  char *ext = strrchr(filename, '.');
  if (ext != NULL) {
    *ext = '\0';
  }
  filename = proper_str_cat(filename, ".output");
  output_file = fopen(filename, "w");
  if (output_file == NULL) {
    perror("Error: Can't open file.\n");
    close_output_file();
    exit(EXIT_FAILURE);
  }
}

void adjust_print(bool is_stdout, const char *format, const char *format_no_newline, ...) {
  va_list args;
  va_start(args, format_no_newline);

  if (test_mode) {
    if (output_file != NULL) {
      vfprintf(output_file, format_no_newline, args);
    } else {
      fprintf(stderr, "Error: File not opened for output\n");
    }
  } else {
    if (is_stdout) {
      vfprintf(stdout, format, args);
    } else {
      vfprintf(stderr, format, args);
    }
  }

  va_end(args);
}

void close_output_file() {
  if (output_file != NULL) {
    fclose(output_file);
    output_file = NULL;
  }
}
