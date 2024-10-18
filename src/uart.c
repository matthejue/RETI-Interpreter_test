#include "../include/uart.h"
#include "../include/globals.h"
#include "../include/reti.h"
#include "../include/utils.h"
#include "../include/parse_args.h"
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t spec = 0;
uint8_t remaining_bytes = 0;
uint8_t num_bytes = 0;

uint8_t *send_data;
char received_num = '\0';

uint8_t sending_waiting_time = 0;
uint8_t receiving_waiting_time = 0;

bool sending_finished = false;
bool receiving_finished = false;

typedef enum { INTEGER, STRING } Datatype;

Datatype datatype;

void uart_send() {
  if (!(read_array(uart, 2, true) & 0b00000001) && !sending_finished) {
    if (spec == 0) {
      spec = uart[0];
      remaining_bytes = spec & 0b01111111;
      num_bytes = remaining_bytes;
      datatype = (spec & 0b10000000) >> 7;
      send_data = malloc(remaining_bytes);
    } else if (remaining_bytes > 0) {
      send_data[num_bytes - remaining_bytes] = uart[0];
      remaining_bytes--;
      if (remaining_bytes == 0) {
        if (datatype == INTEGER) {
          for (int i = 0; i < ceil((double)num_bytes / 4); i++) {
            printf("%d ", (uint32_t)send_data[i * 4]);
          }
          printf("\n");
        } else { // dataype == STRING
          for (int i = 0; i < num_bytes; i++) {
            printf("%c", send_data[i]);
          }
          printf("\n");
        }
        spec = 0;
      }
    }
    if (max_waiting_instrs == 0) {
      sending_waiting_time = 0;
    } else {
      sending_waiting_time = rand() % max_waiting_instrs + 1;
    }
    sending_finished = true;
  } else if (sending_waiting_time > 0 && sending_finished) {
    sending_waiting_time--;
  } else if (sending_waiting_time == 0 && sending_finished) {
    uart[2] = uart[2] | 0b00000001;
    sending_finished = false;
  }
}

void uart_receive() {
  if (!(read_array(uart, 2, true) & 0b00000010) && !receiving_finished) {
    char input[4]; // Buffer to store the input

    while (true) {
      printf("Please enter a number or a character: ");
      // TODO: What happens if more than 4 characters are entered? Error needed for that
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
      receiving_waiting_time = 0;
    } else {
      receiving_waiting_time = rand() % max_waiting_instrs + 1;
    }
    receiving_finished = true;
  } else if (receiving_waiting_time > 0 && receiving_finished) {
    receiving_waiting_time--;
  } else if (receiving_waiting_time == 0 && receiving_finished) {
    uart[1] = received_num; // & 0xFF; not necessary
    uart[2] = uart[2] | 0b00000010;
    receiving_finished = false;
  }
}
