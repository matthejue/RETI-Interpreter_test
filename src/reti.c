#include "../include/reti.h"
#include "../include/globals.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t *regs, *eprom, *uart;

// File pointers for peripheral devices
FILE *sram, *hdd;

// Initialize file-backed storage
void init_reti() {
  regs = malloc(sizeof(uint32_t) * NUM_REGISTERS);
  eprom = malloc(sizeof(uint32_t) * NUM_INSTRUCTIONS_START_PROGRAM);
  uart = malloc(sizeof(uint32_t) * NUM_UART_ADDRESSES);
  memset(regs, 0, sizeof(uint32_t) * NUM_REGISTERS);
  memset(eprom, 0, sizeof(uint32_t) * NUM_INSTRUCTIONS_START_PROGRAM);
  memset(uart, 0, sizeof(uint32_t) * NUM_UART_ADDRESSES);
  sram = fopen("sram.bin", "w+b");
  hdd = fopen("hdd.bin", "w+b");
  if (!sram || !hdd) {
    perror("Failed to open storage files");
    exit(EXIT_FAILURE);
  }
}

uint32_t read_storage(uint32_t *stor, uint16_t addr) {
  return stor[addr];
}

void write_storage(uint32_t *stor, uint16_t addr, uint32_t buffer) {
  stor[addr] = buffer;
}

uint32_t read_file(FILE *dev, uint64_t address) {
  uint32_t buffer;
  fseek(dev, address * sizeof(uint32_t), SEEK_SET);
  fread(&buffer, sizeof(uint32_t), 1, dev);
  return buffer;
}

void write_file(FILE *dev, uint64_t address, uint32_t buffer) {
  fseek(dev, address * sizeof(uint32_t), SEEK_SET);
  fwrite(&buffer, sizeof(uint32_t), 1, dev);
  fflush(dev);
}

// Example function to emulate processor operation
// void emulate_processor() {
//     // Example: Read a value from EPROM and store it in ACC
//     uint32_t value;
//     read_device(eprom, 0x00, &value, sizeof(value));
//     regs.ACC = value;
//
//     // Example: Write a value from ACC to SRAM
//     value = regs.ACC;
//     write_device(sram, 0x00, &value, sizeof(value));
// }

void fin_reti() {
  fclose(sram);
  fclose(hdd);
}
