#ifndef PARSE_H
#define PARSE_H

// Define some basic operations
typedef enum {MOV, ADD, SUB, UNKNOWN} Operation;

// Structure to hold a parsed instruction
typedef struct {
    Operation op;
    char operand1[10];
    char operand2[10];
} Instruction;

// Function declarations
Operation parse_operation(const char *op_str);
Instruction parse_instruction(const char *instr_str);
void process_instruction(Instruction instr);
void parse_program(const char *program);

#endif
