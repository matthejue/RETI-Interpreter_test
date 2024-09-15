#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Define the registers
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

Registers regs;

// File pointers for peripheral devices
FILE *eprom, *uart, *sram, *hdd;

// Initialize file-backed storage
void init_storage() {
    eprom = fopen("eprom.bin", "w+b");
    uart = fopen("uart.bin", "w+b");
    sram = fopen("sram.bin", "w+b");
    hdd = fopen("hdd.bin", "w+b");
    if (!eprom || !uart || !sram || !hdd) {
        perror("Failed to open storage files");
        exit(EXIT_FAILURE);
    }
}

// Read from a device
void read_device(FILE *device, uint32_t address, void *buffer, size_t size) {
    fseek(device, address, SEEK_SET);
    fread(buffer, size, 1, device);
}

// Write to a device
void write_device(FILE *device, uint32_t address, void *buffer, size_t size) {
    fseek(device, address, SEEK_SET);
    fwrite(buffer, size, 1, device);
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

void initialize_reti() {
    memset(&regs, 0, sizeof(Registers));

    init_storage();
}

void finalize() {
    fclose(eprom);
    fclose(uart);
    fclose(sram);
    fclose(hdd);
}
