#ifndef RETI_H
#define RETI_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// Define the structure for registers
typedef struct {
  uint32_t PC;  // Program Counter
  uint32_t ACC; // Accumulator
  uint32_t IN1; // Input Register 1
  uint32_t IN2; // Input Register 2
  uint32_t SP;  // Stack Pointer
  uint32_t BAF; // Base Address Register
  uint32_t CS;  // Code Segment
  uint32_t DS;  // Data Segment
} Registers;

// Declare external variables for the registers and peripheral devices
extern Registers regs;
extern FILE *eprom;
extern FILE *uart;
extern FILE *sram;
extern FILE *hdd;

void read_device(FILE *device, uint32_t address, void *buffer, size_t size) ;
void write_device(FILE *device, uint32_t address, void *buffer, size_t size) ;

void initialize_reti();
void finalize();

#endif // RETI_H
