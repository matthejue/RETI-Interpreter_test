#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"

// Function to identify the operation
Operation parse_operation(const char *op_str) {
    if (strcmp(op_str, "MOV") == 0) return MOV;
    if (strcmp(op_str, "ADD") == 0) return ADD;
    if (strcmp(op_str, "SUB") == 0) return SUB;
    return UNKNOWN;
}

// Function to parse a single instruction string
Instruction parse_instruction(const char *instr_str) {
    Instruction instr;
    char operation[10], operand1[10], operand2[10];
    
    // Parse operation and operands
    sscanf(instr_str, "%s %s %s", operation, operand1, operand2);
    
    // Identify operation
    instr.op = parse_operation(operation);
    strcpy(instr.operand1, operand1);
    strcpy(instr.operand2, operand2);
    
    return instr;
}

// Function to process the instruction (for now, just print it)
void process_instruction(Instruction instr) {
    switch (instr.op) {
        case MOV:
            printf("MOV %s, %s\n", instr.operand1, instr.operand2);
            break;
        case ADD:
            printf("ADD %s, %s\n", instr.operand1, instr.operand2);
            break;
        case SUB:
            printf("SUB %s, %s\n", instr.operand1, instr.operand2);
            break;
        default:
            printf("Unknown instruction\n");
    }
}

// Function to split the input by semicolons and parse each instruction
void parse_program(const char *program) {
    char *program_copy = strdup(program);  // Make a mutable copy of the program
    char *instr_str = strtok(program_copy, ";");
    
    while (instr_str != NULL) {
        // Parse and process each instruction
        Instruction instr = parse_instruction(instr_str);
        process_instruction(instr);
        
        // Get the next instruction
        instr_str = strtok(NULL, ";");
    }
    
    free(program_copy);
}
