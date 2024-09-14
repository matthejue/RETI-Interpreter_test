#include <stdio.h>
#include "parse.h"  // Include the header file for the parsing functionality

int main() {
    // Example program input
    const char *program = "MOV R0, R1; ADD R1, 123; SUB R2, R0;";
    
    // Parse and process the program
    parse_program(program);
    
    return 0;
}
