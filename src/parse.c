#include "../include/parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
            printf("MOV %s %s\n", instr.operand1, instr.operand2);
            break;
        case ADD:
            printf("ADD %s %s\n", instr.operand1, instr.operand2);
            break;
        case SUB:
            printf("SUB %s %s\n", instr.operand1, instr.operand2);
            break;
        default:
            printf("Unknown instruction\n");
    }
}

// // Function to split the input by semicolons and parse each instruction
// void parse_program(const char *program) {
//     char *program_copy = strdup(program);  // Make a mutable copy of the program
//     char *instr_str = strtok(program_copy, ";");
//
//     while (instr_str != NULL || instr_str == "") {
//         // Parse and process each instruction
//         Instruction instr = parse_instruction(instr_str);
//         process_instruction(instr);
//
//         // Get the next instruction
//         instr_str = strtok(NULL, ";");
//     }
//
//     free(program_copy);
// }

// Function to trim leading and trailing white spaces
char *trim_whitespace(char *str) {
    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator
    *(end + 1) = '\0';

    return str;
}

// Function to tokenize the input string based on delimiters "\n" and ";"
char **tokenize(const char *input, const char *delimiters, int *count) {
    char *input_copy = strdup(input);
    char *token;
    char **tokens = NULL;
    int tokens_count = 0;

    token = strtok(input_copy, delimiters);
    while (token != NULL) {
        tokens = realloc(tokens, sizeof(char *) * (tokens_count + 1));
        tokens[tokens_count++] = strdup(token);
        token = strtok(NULL, delimiters);
    }

    free(input_copy);
    *count = tokens_count;
    return tokens;
}

// Function to parse the program
void parse_program(const char *program) {
    int count;
    char **instructions = tokenize(program, "\n;", &count);

    for (int i = 0; i < count; i++) {
        char *instruction = trim_whitespace(instructions[i]);
        if (strlen(instruction) > 0) {
            // Parse the instruction
            printf("Parsed instruction: '%s'\n", instruction);
        }
        free(instructions[i]);
    }
    free(instructions);
}
