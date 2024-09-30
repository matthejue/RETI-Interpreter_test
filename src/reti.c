#include "../include/reti.h"
#include "../include/assemble.h"
#include "../include/globals.h"
#include "../include/utils.h"
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

uint32_t read_array(uint32_t *stor, uint16_t addr) { return stor[addr]; }

void write_array(uint32_t *stor, uint16_t addr, uint32_t buffer) {
  stor[addr] = buffer;
}

uint32_t read_file(FILE *dev, uint64_t address) {
  uint32_t big_endian_buffer;
  fseek(dev, address * sizeof(uint32_t), SEEK_SET);
  fread(&big_endian_buffer, sizeof(uint32_t), 1, dev);
  return swap_endian_32(big_endian_buffer);
}

void write_file(FILE *dev, uint64_t addr, uint32_t buffer) {
  uint32_t big_endian_buffer = swap_endian_32(buffer);
  fseek(dev, addr * sizeof(uint32_t), SEEK_SET);
  fwrite(&big_endian_buffer, sizeof(uint32_t), 1, dev);
  fflush(dev);
}

uint32_t read_storage_ds_fill(uint32_t addr) {
  addr = addr | (read_array(regs, DS) & 0xffc00000);
  return read_storage(addr);
}

uint32_t read_storage(uint32_t addr) {
  uint8_t stor_mode = addr >> 30;
  switch (stor_mode) {
  case 0b00:
    addr = addr & 0x3FFFFFF;
    return read_array(eprom, addr);
  case 0b01:
    addr = addr & 0x3FFFFFF;
    return read_array(uart, addr);
  default:
    addr = addr & 0x7FFFFFF;
    return read_file(sram, addr);
  }
}

void write_storage_ds_fill(uint64_t addr, uint32_t buffer) {
  addr = addr | (read_array(regs, DS) & 0xffc00000);
  write_storage(addr, buffer);
}

void write_storage(uint32_t addr, uint32_t buffer) {
  uint8_t stor_mode = addr >> 30;
  addr = addr & 0x3FFFFFF;
  switch (stor_mode) {
  case 0b00:
    addr = addr & 0x3FFFFFF;
    write_array(eprom, addr, buffer);
  case 0b01:
    addr = addr & 0x3FFFFFF;
    write_array(uart, addr, buffer);
  default:
    addr = addr & 0x7FFFFFF;
    write_file(sram, addr, buffer);
  }
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
