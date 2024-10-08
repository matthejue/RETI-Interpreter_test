#ifndef RETI_H
#define RETI_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

extern uint32_t *regs;
extern uint32_t *eprom;
extern uint32_t *uart;
extern FILE *sram;
extern FILE *hdd;

extern uint64_t sram_view_pos;
extern uint64_t hdd_view_pos;

void load_adjusted_eprom_prgrm();
void load_isrs();

uint32_t read_file(FILE *dev, uint64_t address);
void write_file(FILE *dev, uint64_t address, uint32_t buffer);

uint32_t read_array(uint32_t *stor, uint16_t addr);
void write_array(uint32_t *stor, uint16_t addr, uint32_t buffer);

uint32_t read_storage_ds_fill(uint32_t addr);
uint32_t read_storage(uint32_t addr);
void write_storage_ds_fill(uint64_t addr, uint32_t buffer);
void write_storage(uint32_t addr, uint32_t buffer);

void init_reti();
void fin_reti();

#endif // RETI_H
