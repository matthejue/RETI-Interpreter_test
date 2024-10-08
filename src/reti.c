#include "../include/reti.h"
#include "../include/assemble.h"
#include "../include/globals.h"
#include "../include/utils.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t *regs, *eprom, *uart;

FILE *sram, *hdd;

uint32_t num_instrs_prgrm = 0;
uint32_t num_instrs_start_prgrm = 0;
uint32_t num_instrs_isrs = 0;

void init_reti() {
  regs = malloc(sizeof(uint32_t) * NUM_REGISTERS);
  // TODO: herausfinden, wie man num_instrs_start_prgrm vorher bestimmt
  eprom = malloc(sizeof(uint32_t) * num_instrs_start_prgrm);
  uart = malloc(sizeof(uint32_t) * NUM_UART_ADDRESSES);

  memset(regs, 0, sizeof(uint32_t) * NUM_REGISTERS);
  memset(uart, 0, sizeof(uint32_t) * NUM_UART_ADDRESSES);

  // TODO: Tobias: Die ganzen Speicher nicht mit 0 initialisiert
  char *file_path = proper_str_cat(peripherals_dir, "/sram.bin");
  sram = fopen(file_path, "w+b");

  file_path = proper_str_cat(peripherals_dir, "/hdd.bin");
  hdd = fopen(file_path, "w+b");

  if (!sram || !hdd) {
    perror("Failed to open storage files");
    exit(EXIT_FAILURE);
  }
}

void load_adjusted_eprom_prgrm() {
  uint8_t i = 0;
  String_Instruction str_instr = {
      .op = "LOADI", .opd1 = "SP", .opd2 = "-1", .opd3 = ""};
  write_array(eprom, i++, assembly_to_machine(&str_instr));
  str_instr = (String_Instruction){
      .op = "MULTI", .opd1 = "SP", .opd2 = "1024", .opd3 = ""};
  write_array(eprom, i++, assembly_to_machine(&str_instr));
  str_instr = (String_Instruction){
      .op = "ADDI", .opd1 = "SP", .opd2 = "1023", .opd3 = ""};
  write_array(eprom, i++, assembly_to_machine(&str_instr));

  str_instr = (String_Instruction){
      .op = "MOVE", .opd1 = "SP", .opd2 = "BAF", .opd3 = ""};
  write_array(eprom, i++, assembly_to_machine(&str_instr));

  // Codesegment register
  uint16_t im = 0b10 << 30 | num_instrs_isrs;
  // or because of assumption that num_instrs_isrs is less than 2^30
  uint16_t im_upper_str = sign_extend_10_to_32(im >> 10);
  uint32_t im_lower_str = im & IMMEDIATE_MASK;
  str_instr =
      (String_Instruction){.op = "LOADI", .opd1 = "CS", .opd2 = im_upper_str, .opd3 = ""};
  write_array(eprom, i++, assembly_to_machine(&str_instr));
  str_instr =
      (String_Instruction){.op = "MULTI", .opd1 = "CS", .opd2 = "2048", .opd3 = ""};
  write_array(eprom, i++, assembly_to_machine(&str_instr));
  str_instr =
      (String_Instruction){.op = "ORI", .opd1 = "CS", .opd2 = im_lower_str, .opd3 = ""};
  write_array(eprom, i++, assembly_to_machine(&str_instr));
  str_instr = (String_Instruction){
      .op = "MOVE", .opd1 = "CS", .opd2 = "DS", .opd3 = ""};
  write_array(eprom, i++, assembly_to_machine(&str_instr));

  // Datensegment register
  im_upper_str = sign_extend_10_to_32(num_instrs_prgrm >> 10);
  im_lower_str = num_instrs_prgrm & IMMEDIATE_MASK;
  str_instr =
      (String_Instruction){.op = "LOADI", .opd1 = "ACC", .opd2 = im_upper_str, .opd3 = ""};
  write_array(eprom, i++, assembly_to_machine(&str_instr));
  str_instr =
      (String_Instruction){.op = "MULTI", .opd1 = "ACC", .opd2 = "2048", .opd3 = ""};
  write_array(eprom, i++, assembly_to_machine(&str_instr));
  str_instr =
      (String_Instruction){.op = "ORI", .opd1 = "ACC", .opd2 = im_lower_str, .opd3 = ""};
  write_array(eprom, i++, assembly_to_machine(&str_instr));
  str_instr =
      (String_Instruction){.op = "ADD", .opd1 = "DS", .opd2 = "ACC", .opd3 = ""};
  write_array(eprom, i++, assembly_to_machine(&str_instr));

  str_instr = (String_Instruction){
      .op = "MOVE", .opd1 = "CS", .opd2 = "PC", .opd3 = ""};
  write_array(eprom, i++, assembly_to_machine(&str_instr));

  // TODO: Tobias, soll eprom schreiben ab hier gelockt sein?
  num_instrs_isrs = i;
}

void load_isrs() {}

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
