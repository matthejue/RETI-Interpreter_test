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

void write_storage(uint32_t *stor, uint16_t addr, uint32_t buffer) ;
uint32_t read_storage(uint32_t *stor, uint16_t addr) ;

uint32_t read_file(FILE *dev, uint64_t address);
void write_file(FILE *dev, uint64_t address, uint32_t buffer);

void init_reti();
void fin_reti();

#endif // RETI_H
