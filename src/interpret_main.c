// #include <stdio.h>
#include "../include/globals.h"
#include "../include/parse.h"
#include "../include/reti.h"
#include "../include/interpret.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

uint32_t ram_size = 4294967295;
uint16_t page_size = 4096;
uint32_t hdd_size = 4294967295;

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

void parse_arguments(uint8_t argc, char *argv[], char **input) {
  uint32_t opt;
  *input = NULL;

  while ((opt = getopt(argc, argv, "r:p:h:")) != -1) {
    switch (opt) {
    case 'r':
      ram_size = atoi(optarg);
      break;
    case 'p':
      page_size = atoi(optarg);
      break;
    case 'h':
      hdd_size = atoi(optarg);
      break;
    default:
      fprintf(stderr, "Usage: %s -r ram_size -p page_size -h hdd_size input\n",
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

int main(int argc, char *argv[]) {
  char *input = NULL;

  parse_arguments(argc, argv, &input);

  printf("RAM Size: %d\n", ram_size);
  printf("Page Size: %d\n", page_size);
  printf("HDD Size: %d\n", hdd_size);
  printf("Input: %s\n", input);

  init_reti();

  parse_and_load_program(input);

  interpr_program();

  fin_reti();

  free(input);
  return 0;
}

// TODO:
// - cli options:
//   - RAM size
//   - page size
// - reti, eprom, uart, sram, sata
//   - Befehle am Anfang reinladen ins CS
// - paging table generieren
// - eprom, regs und uart keine Dateien, nur sram und hdd
// - Startprogram bei der Kompilierung schon vorher erstellen
// - Kommentare supporten
// - INT und RETI
// - Test machen, der alle Instructions einmal ausführt
// - Memory Map über DS Register steuerbar machen
