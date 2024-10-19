#include <stdint.h>

#define UART_H
#ifdef UART_H

extern uint8_t remaining_bytes;
extern uint8_t num_bytes;
extern uint8_t *send_data;
extern uint16_t str_idx;

extern uint8_t sending_waiting_time;
extern uint8_t receiving_waiting_time;


typedef enum { STRING, INTEGER } DataType;

extern DataType datatype;

void uart_send();
void uart_receive();

#endif // UART_H
