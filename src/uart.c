#include "../include/globals.h"
#include "../include/reti.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint8_t specification = 0;
uint8_t remaining_bytes = 0;
uint8_t num_bytes = 0;
uint8_t *received_data;
uint8_t waiting_time = 0;

bool reading_finished = false;

typedef enum { INTEGER, STRING } Datatype;

Datatype datatype;

void uart_check() {
  if (!(read_array(uart, 2, true) & 0b00000001) && !reading_finished) {
    if (specification == 0) {
      specification = read_array(uart, 0, true);
      remaining_bytes = specification & 0b01111111;
      num_bytes = remaining_bytes;
      datatype = (specification & 0b10000000) >> 7;
      received_data = malloc(remaining_bytes);
    } else if (remaining_bytes > 0) {
      received_data[num_bytes - remaining_bytes] = read_array(uart, 0, true);
      remaining_bytes--;
      if (remaining_bytes == 0) {
        if (datatype == INTEGER) {
          for (int i = 0; i < ceil((double)num_bytes / 4); i++) {
            printf("%d ", (uint32_t)received_data[i * 4]);
          }
          printf("\n");
        } else { // dataype == STRING
          for (int i = 0; i < num_bytes; i++) {
            printf("%c", received_data[i]);
          }
          printf("\n");
        }
        specification = 0;
      }
    }
    // randomly pick number btween 1 and 5
    waiting_time = rand() % MAX_WAITING_TIME + 1;
    reading_finished = true;
  } else if (waiting_time > 0 && reading_finished) {
    waiting_time--;
  } else if (waiting_time == 0 && reading_finished) {
    // to make it more realistic the bit is not set to 1 again immediately
    uart[2] = 0b00000001;
    reading_finished = false;
  }
}
