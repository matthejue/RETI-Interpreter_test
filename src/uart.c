#include "../include/uart.h"
#include "../include/globals.h"
#include "../include/parse_args.h"
#include "../include/reti.h"
#include "../include/utils.h"
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t remaining_bytes = 0;
uint8_t num_bytes = 0;

uint16_t str_idx = 0;

uint8_t *send_data;
char received_num = '\0';

uint8_t sending_waiting_time = 0;
uint8_t receiving_waiting_time = 0;

bool sending_finished = false;
bool receiving_finished = false;

bool init_finished = false;

DataType datatype;

void uart_send() {
  if (!(read_array(uart, 2, true) & 0b00000001) && !sending_finished) {
    if (!init_finished) {
      datatype = uart[0];
      switch (datatype) {
      case STRING:
        remaining_bytes = 0;
        send_data = NULL;
        break;
      case INTEGER:
        datatype = INTEGER;
        num_bytes = remaining_bytes = 4;
        send_data = malloc(remaining_bytes);
        memset(send_data, 0, remaining_bytes);
        break;
      }
      init_finished = true;
    } else if (datatype == INTEGER) {
      send_data[num_bytes - remaining_bytes] = uart[0];
      remaining_bytes--;
      if (remaining_bytes == 0) {
        printf("%d\n", swap_endian_32(*((uint32_t *)(send_data))));

        init_finished = false;
      }
    } else if (datatype == STRING) {
      send_data = realloc(send_data, 1);
      uint8_t data = uart[0];
      send_data[str_idx] = data;
      str_idx++;
      if (data == 0) {
        printf("%s\n", send_data);

        str_idx = 0;
        init_finished = false;
      }
    } else {
      perror("Error: Invalid datatype.\n");
      exit(EXIT_FAILURE);
    }

    if (max_waiting_instrs == 0) {
      goto sending_finished;
    } else {
      sending_waiting_time = rand() % max_waiting_instrs + 1;
    }
    sending_finished = true;
  } else if (sending_finished) {
  sending_finished:
    sending_waiting_time--;
    if (sending_waiting_time == 0) {
      uart[2] = uart[2] | 0b00000001;
      sending_finished = false;
    }
  }
}

void uart_receive() {
  if (!(read_array(uart, 2, true) & 0b00000010) && !receiving_finished) {
    char input[4]; // Buffer to store the input

    while (true) {
      printf("Please enter a number or a character: ");
      // TODO: What happens if more than 4 characters are entered? Error needed
      // for that
      if (fgets(input, sizeof(input), stdin) == NULL) {
        perror("Error: Couldn't read input.\n");
      }
      input[strcspn(input, "\n")] = '\0';

      if (isalpha(input[0]) && !receiving_finished) {
        if (strlen(input) > 1) {
          perror("Error: Only one character allowed.\n");
        } else {
          received_num = input[0];
          break;
        }
      } else if (isdigit(input[0])) {
        char *endptr;
        received_num = strtol(input, &endptr, 10);
        if (*endptr != '\0') {
          const char *str = "Error: Further characters after number: ";
          const char *str2 = proper_str_cat(str, endptr);
          const char *str3 = proper_str_cat(str2, ".\n");
          perror(str3);
        } else if (received_num < INT8_MIN && received_num > INT8_MAX) {
          perror("Error: Number out of range, must be between -128 and 127.\n");
        } else {
          break;
        }
      } else {
        perror("Error: Invalid input.\n");
      }
    }
    if (max_waiting_instrs == 0) {
      goto receiving_finished;
    } else {
      receiving_waiting_time = rand() % max_waiting_instrs + 1;
    }
    receiving_finished = true;
  } else if (receiving_finished) {
  receiving_finished:
    receiving_waiting_time--;
    if (receiving_waiting_time == 0) {
      uart[1] = received_num; // & 0xFF; not necessary
      uart[2] = uart[2] | 0b00000010;
      receiving_finished = false;
    }
  }
}
