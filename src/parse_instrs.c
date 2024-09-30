#include "../include/parse_instrs.h"
#include "../include/globals.h"
#include "../include/interpr.h"
#include "../include/reti.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t num_instrs_prgrm = 0;

// TODO: Irgendwie durch untere Funktionen dafür sorgen, dass immer nur ein
// TODO: nen check machen, ob Zahl nicht zu lang, später in ner anderen

String_Instruction *parse_instr(const char **original_prgrm_pntr) {
  const char *prgrm_pntr = *original_prgrm_pntr;
  String_Instruction *str_instr = malloc(sizeof(String_Instruction));
  uint8_t token_cnt = 0;
  uint8_t rel_idx;

  while (*prgrm_pntr == ' ') {
    prgrm_pntr++;
  }

  while (true) {
    rel_idx = 0;
    while (true) {
      if (*prgrm_pntr == ' ') {
        switch (token_cnt) {
        case 0:
          (str_instr->op)[rel_idx] = '\0';
          break;
        case 1:
          (str_instr->opd1)[rel_idx] = '\0';
          break;
        case 2:
          (str_instr->opd2)[rel_idx] = '\0';
          break;
        case 3:
          (str_instr->opd3)[rel_idx] = '\0';
          break;
        }
        while (*prgrm_pntr == ' ') {
          prgrm_pntr++;
        }
        break;
      } else if (*prgrm_pntr == ';' || *prgrm_pntr == '\n') {
        prgrm_pntr++;
        *original_prgrm_pntr = prgrm_pntr;
        return str_instr;
      } else if (*prgrm_pntr == '\0') {
        *original_prgrm_pntr = prgrm_pntr;
        return str_instr;
      }
      switch (token_cnt) {
      case 0:
        (str_instr->op)[rel_idx] = *prgrm_pntr;
        break;
      case 1:
        (str_instr->opd1)[rel_idx] = *prgrm_pntr;
        break;
      case 2:
        (str_instr->opd2)[rel_idx] = *prgrm_pntr;
        break;
      case 3:
        (str_instr->opd3)[rel_idx] = *prgrm_pntr;
        break;
      }
      rel_idx++;
      prgrm_pntr++;
    }
    token_cnt++;
  }
}

void parse_and_load_program(char *prgrm, Memory_Type memory_type) {
  const char *prgrm_pntr = prgrm;
  uint32_t i = 0;

  while (*prgrm_pntr != '\0') {
    String_Instruction *str_instr = parse_instr(&prgrm_pntr);
    if (isalpha(*str_instr->op)) {
      uint32_t machine_instr = assembly_to_machine(str_instr);
      switch (memory_type) {
        case EPROM:
          write_array(eprom, i++, machine_instr);
          break;
        case SRAM:
          write_file(sram, i++, machine_instr);
          break;
        default:
          perror("Error: Invalid memory type");
          break;
      }
    }
  }
  num_instrs_prgrm = i;
  free(prgrm);
}
