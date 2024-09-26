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
  // eprom = malloc(sizeof(uint32_t) * NUM_INSTRUCTIONS_START_PROGRAM);
  uart = malloc(sizeof(uint32_t) * NUM_UART_ADDRESSES);
  memset(regs, 0, sizeof(uint32_t) * NUM_REGISTERS);
  // memset(eprom, 0, sizeof(uint32_t) * NUM_INSTRUCTIONS_START_PROGRAM);
  memset(uart, 0, sizeof(uint32_t) * NUM_UART_ADDRESSES);

  char *suffix = "/sram.bin";
  char *new_file_dir = malloc(strlen(file_dir) + strlen(suffix) + 1);
  strcpy(new_file_dir, file_dir);
  sram = fopen(strcat(new_file_dir, suffix), "w+b");

  suffix = "/hdd.bin";
  new_file_dir = malloc(strlen(file_dir) + strlen(suffix) + 1);
  strcpy(new_file_dir, file_dir);
  hdd = fopen(strcat(new_file_dir, suffix), "w+b");
  if (!sram || !hdd) {
    perror("Failed to open storage files");
    exit(EXIT_FAILURE);
  }
}

uint32_t read_storage(uint32_t *stor, uint16_t addr) { return stor[addr]; }

void write_storage(uint32_t *stor, uint16_t addr, uint32_t buffer) {
  stor[addr] = buffer;
}

uint32_t swap_endian_32(uint32_t value) {
  return (value >> 24) | ((value >> 8) & 0x0000FF00) |
         ((value << 8) & 0x00FF0000) | (value << 24);
}

uint32_t read_file(FILE *dev, uint64_t address) {
  uint32_t big_endian_buffer;
  fseek(dev, address * sizeof(uint32_t), SEEK_SET);
  fread(&big_endian_buffer, sizeof(uint32_t), 1, dev);
  return swap_endian_32(big_endian_buffer);
}

void write_file(FILE *dev, uint64_t address, uint32_t buffer) {
  uint32_t big_endian_buffer = swap_endian_32(buffer);
  fseek(dev, address * sizeof(uint32_t), SEEK_SET);
  fwrite(&big_endian_buffer, sizeof(uint32_t), 1, dev);
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
