// #include <stdio.h>
#include "../include/globals.h"
#include "../include/interpr.h"
#include "../include/parse_args.h"
#include "../include/reti.h"
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

uint32_t sram_size = 4294967295;
uint16_t page_size = 4096;
uint32_t hdd_size = 4294967295;
bool daemon_mode = false;
uint8_t radius = 32;
char *file_dir = ".";

char *read_file_content(const char *file_path) {
  FILE *file = fopen(file_path, "r");
  if (!file) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *content = malloc(file_size + 1);
  if (!content) {
    perror("Error allocating memory");
    exit(EXIT_FAILURE);
  }

  fread(content, 1, file_size, file);
  content[file_size] = '\0';

  fclose(file);
  return content;
}

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

void parse_args(uint8_t argc, char *argv[], char **input) {
  uint32_t opt;
  *input = NULL;

  while ((opt = getopt(argc, argv, "s:p:h:d:r:f:")) != -1) {
    char *endptr;
    errno = 0;
    long tmp_val;

    switch (opt) {
    case 's':
      tmp_val = strtol(optarg, &endptr, 10);
      if (endptr == optarg || *endptr != '\0' || errno == ERANGE) {
        perror("Error: Invalid sram size");
        exit(EXIT_FAILURE);
      }
      if (tmp_val < 0 || tmp_val > UINT32_MAX) {
        perror("Error: SRAM size must be between 0 and 4294967295");
        exit(EXIT_FAILURE);
      }
      sram_size = tmp_val;
      break;
    case 'p':
      tmp_val = strtol(optarg, &endptr, 10);
      if (endptr == optarg || *endptr != '\0' || errno == ERANGE) {
        perror("Error: Invalid page size");
        exit(EXIT_FAILURE);
      }
      if (page_size < 0 || page_size > UINT16_MAX) {
        perror("Error: Page size must be between 0 and 65535");
        exit(EXIT_FAILURE);
      }
      page_size = tmp_val;
      break;
    case 'h':
      tmp_val = strtol(optarg, &endptr, 10);
      if (endptr == optarg || *endptr != '\0' || errno == ERANGE) {
        perror("Error: Invalid hdd size");
        exit(EXIT_FAILURE);
      }
      if (tmp_val < 0 || tmp_val > UINT32_MAX) {
        perror("Error: HDD size must be between 0 and 4294967295");
        exit(EXIT_FAILURE);
      }
      hdd_size = tmp_val;
      break;
    case 'd':
      daemon_mode = true;
      break;
    case 'r':
      tmp_val = strtol(optarg, &endptr, 10);
      if (endptr == optarg || *endptr != '\0' || errno == ERANGE) {
        perror("Error: Invalid radius size");
        exit(EXIT_FAILURE);
      }
      if (tmp_val < 8 || tmp_val > UINT8_MAX) {
        perror("Error: Radius must be between 8 and 255");
        exit(EXIT_FAILURE);
      }
      radius = tmp_val;
      break;
    case 'f':
      file_dir = optarg;
      break;
    default:
      fprintf(
          stderr,
          "Usage: %s -r ram_size -p page_size -h hdd_size -d (run as daemon)"
          "-r radius -f file_dir input\n",
          argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  if (optind >= argc) {
    fprintf(stderr, "Expected argument after options\n");
    exit(EXIT_FAILURE);
  }

  const char *input_arg = argv[optind];

  // Check if the input is a file path or a string piped over stdout
  if (strcmp(input_arg, "-") == 0) {
    *input = read_stdin_content();
  } else {
    FILE *file = fopen(input_arg, "r");
    if (file) {
      fclose(file);
      *input = read_file_content(input_arg);
    } else {
      fprintf(stderr, "Error: Unable to open file %s\n", input_arg);
      exit(EXIT_FAILURE);
    }
  }
}
