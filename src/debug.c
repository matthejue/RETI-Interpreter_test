#include "../include/debug.h"
#include "../include/assemble.h"
#include "../include/parse_args.h"
#include "../include/reti.h"
#include "../include/uart.h"
#include "../include/utils.h"
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define clrscr() system("cls")
#else
#define clrscr() printf("\e[1;1H\e[2J") // clear sequence for ANSI terminals
#endif

bool breakpoint_encountered = true;
bool step_into_activated = false;
bool isr_active = false;

bool invalid_input = false;

const uint8_t LINEWIDTH = 54;

Mnemonic_to_String opcode_to_mnemonic[] = {
    {ADDI, "ADDI"},     {SUBI, "SUBI"},       {MULTI, "MULTI"},
    {DIVI, "DIVI"},     {MODI, "MODI"},       {OPLUSI, "OPLUSI"},
    {ORI, "ORI"},       {ANDI, "ANDI"},       {ADDR, "ADD"},
    {SUBR, "SUB"},      {MULTR, "MULT"},      {DIVR, "DIV"},
    {MODR, "MOD"},      {OPLUSR, "OPLUS"},    {ORR, "OR"},
    {ANDR, "AND"},      {ADDM, "ADD"},        {SUBM, "SUB"},
    {MULTM, "MULT"},    {DIVM, "DIV"},        {MODM, "MOD"},
    {OPLUSM, "OPLUS"},  {ORM, "OR"},          {ANDM, "AND"},
    {LOAD, "LOAD"},     {LOADIN, "LOADIN"},   {LOADI, "LOADI"},
    {STORE, "STORE"},   {STOREIN, "STOREIN"}, {MOVE, "MOVE"},
    {JUMPGT, "JUMP>"},  {JUMPEQ, "JUMP=="},   {JUMPEQ, "JUMP="},
    {JUMPGE, "JUMP>="}, {JUMPLT, "JUMP<"},    {JUMPNE, "JUMP!="},
    {JUMPNE, "JUMP<>"}, {JUMPLE, "JUMP<="},   {JUMP, "JUMP"},
    {INT, "INT"},       {RTI, "RTI"},         {NOP, "NOP"}};

const uint8_t mem_type_to_constant[] = {0b11, 0b00, 0b01, 0b10, 0b11};

char *copy_mnemonic_into_str(char *dest, const uint8_t opcode) {
  strcat(dest, opcode_to_mnemonic[opcode].name);
  return dest + strlen(dest);
}

char *copy_reg_into_str(char *dest, const uint8_t reg) {
  strcat(dest, " ");
  dest = strcat(dest, register_code_to_name[reg]);
  return dest + strlen(dest);
}

char *copy_im_into_str(char *dest, const uint32_t im) {
  strcpy(dest, " ");
  if (binary_mode) {
    sprintf(dest + 1, "%s", int_to_bin_str(im, 22));
  } else {
    sprintf(dest + 1, "%d", im);
  }
  return dest + strlen(dest);
}

char *assembly_to_str(Instruction *instr) {
  char *instr_str;
  if (binary_mode) {
    instr_str = malloc(39); // STOREIN ACC IN2 22bit\0
  } else {
    instr_str = malloc(25); // STOREIN ACC IN2 -2097152\0
  }
  instr_str[0] = '\0';
  char *dest = instr_str;
  for (size_t i = 0;
       i < sizeof(opcode_to_mnemonic) / sizeof(opcode_to_mnemonic[0]); i++) {
    if (opcode_to_mnemonic[i].value == instr->op) {
      dest = copy_mnemonic_into_str(dest, i);
      break;
    }
  }
  if ((ADDI <= instr->op && instr->op <= ANDI) ||
      (ADDM <= instr->op && instr->op <= ANDM)) {
    dest = copy_reg_into_str(dest, instr->opd1);
    dest = copy_im_into_str(dest, instr->opd2);
  } else if (ADDR <= instr->op && instr->op <= ANDR) {
    dest = copy_reg_into_str(dest, instr->opd1);
    dest = copy_reg_into_str(dest, instr->opd2);
  } else if (instr->op == LOAD || instr->op == STORE || instr->op == LOADI) {
    dest = copy_reg_into_str(dest, instr->opd1);
    dest = copy_im_into_str(dest, instr->opd2);
  } else if (instr->op == LOADIN || instr->op == STOREIN) {
    dest = copy_reg_into_str(dest, instr->opd1);
    dest = copy_reg_into_str(dest, instr->opd2);
    dest = copy_im_into_str(dest, instr->opd3);
  } else if (instr->op == MOVE) {
    dest = copy_reg_into_str(dest, instr->opd1);
    dest = copy_reg_into_str(dest, instr->opd2);
  } else if ((JUMPGT <= instr->op && instr->op <= JUMP) || instr->op == INT) {
    dest = copy_im_into_str(dest, instr->opd1);
  } else if (instr->op == RTI || instr->op == NOP) {
  } else {
    fprintf(stderr, "Invalid instruction\n");
    exit(EXIT_FAILURE);
  }
  return instr_str;
}

char *mem_value_to_str(int32_t mem_content, bool is_unsigned) {
  char *instr_str = malloc(12); // -2147483649
  if (is_unsigned) {
    sprintf(instr_str, "%u", mem_content);
  } else {
    sprintf(instr_str, "%d", mem_content);
  }
  return instr_str;
}

char *mem_value_to_bin_str(uint32_t mem_content) {
  return int_to_bin_str(mem_content, 32);
}

char *reg_to_mem_pntr(uint64_t idx, MemType mem_type) {
  char *active_regs = "";
  bool at_least_one_reg = false;
  for (int i = 0; i < NUM_REGISTERS; i++) {
    uint32_t addr = read_array(regs, i, false);
    uint8_t addr_mem_type = addr >> 30;
    uint32_t addr_idx;
    if (mem_type == SRAM) {
      addr_idx = addr & 0x7FFFFFFF;
    } else {
      addr_idx = addr & 0x3FFFFFFF;
    }
    if ((addr_mem_type == 0b11
             ? mem_type_to_constant[mem_type] == SRAM_CONST
             : addr_mem_type == mem_type_to_constant[mem_type]) &&
        addr_idx == idx) {
      active_regs = proper_str_cat(active_regs, register_code_to_name[i]);
      active_regs = proper_str_cat(active_regs, " ");
      at_least_one_reg = true;
    }
  }
  if (at_least_one_reg) {
    return proper_str_cat("<- ", active_regs);
  }
  return "";
}

// TODO:: split zwischen mem content und assembly instrs
// TODO:: Unit test dafür und die ganzen idx Funktionen
void print_mem_content_with_idx(uint64_t idx, uint32_t mem_content,
                                bool are_unsigned, bool are_instrs,
                                MemType mem_type) {
  char idx_str[20];
  switch (mem_type) {
  case SRAM:
    snprintf(
        idx_str, sizeof(idx_str),
        proper_str_cat(
            proper_str_cat("%0", num_digits_for_idx_str(sram_size - 1)), "zu"),
        idx);
    break;
  // case HDD:
  //   snprintf(idx_str, sizeof(idx_str),
  //            proper_str_cat(
  //                proper_str_cat("%0", num_digits_for_idx_str(hdd_size)), "zu"),
  //            idx);
  //   break;
  case EPROM:
    snprintf(idx_str, sizeof(idx_str),
             proper_str_cat(proper_str_cat("%0", num_digits_for_idx_str(
                                                     num_instrs_start_prgrm)),
                            "zu"),
             idx);
    break;
  case UART:
    snprintf(idx_str, sizeof(idx_str),
             proper_str_cat(proper_str_cat("%0", num_digits_for_idx_str(
                                                     NUM_UART_ADDRESSES)),
                            "zu"),
             idx);
    break;
  default:
    fprintf(stderr, "Error: Invalid memory type\n");
    exit(EXIT_FAILURE);
  }
  const char *mem_content_str;
  if (are_instrs) {
    mem_content_str = assembly_to_str(machine_to_assembly(mem_content));
  } else {
    if (binary_mode) {
      mem_content_str = mem_value_to_bin_str(mem_content);
    } else {
      mem_content_str = mem_value_to_str(mem_content, are_unsigned);
    }
  }
  printf("%s: %s%s\n", idx_str, mem_content_str,
         reg_to_mem_pntr(idx, mem_type));
}

void print_reg_content_with_reg(uint8_t idx, uint32_t mem_content) {
  char reg_str[4];
  snprintf(reg_str, sizeof(reg_str), "%3s", register_code_to_name[idx]);
  const char *mem_content_str_unsigned;
  if (binary_mode) {
    mem_content_str_unsigned = mem_value_to_bin_str(mem_content);
  } else {
    mem_content_str_unsigned = mem_value_to_str(mem_content, true);
  }
  const char *mem_content_str_signed = mem_value_to_str(mem_content, false);
  printf("%s: %s (%s)\n", reg_str, mem_content_str_unsigned,
         mem_content_str_signed);
}

void print_array_with_idcs(MemType mem_type, uint8_t length, bool are_instrs) {
  print_array_with_idcs_from_to(mem_type, 0, length - 1, are_instrs);
}

void print_array_with_idcs_from_to(MemType mem_type, uint64_t start,
                                   uint64_t end, bool are_instrs) {
  switch (mem_type) {
  case REGS:
    for (uint8_t i = start; i <= end; i++) {
      print_reg_content_with_reg(i, ((uint32_t *)regs)[i]);
    }
    break;
  case EPROM:
    for (uint64_t i = start; i <= end; i++) {
      print_mem_content_with_idx(i, ((uint32_t *)eprom)[i], false, are_instrs,
                                 EPROM);
    }
    break;
  case UART:
    for (uint8_t i = start; i <= end; i++) {
      print_mem_content_with_idx(i, ((uint8_t *)uart)[i], false, are_instrs,
                                 UART);
    }
    break;
  default:
    fprintf(stderr, "Error: Invalid memory type\n");
    exit(EXIT_FAILURE);
  }
}

void print_file_with_idcs(MemType mem_type, uint64_t start, uint64_t end,
                          bool are_unsigned, bool are_instrs) {
  switch (mem_type) {
  case SRAM:
    for (uint64_t i = start; i <= end; i++) {
      print_mem_content_with_idx(i, read_file(sram, i), are_unsigned,
                                 are_instrs, SRAM);
    }
    break;
  // case HDD:
  //   for (uint64_t i = start; i <= end; i++) {
  //     print_mem_content_with_idx(i, read_file(hdd, i), are_unsigned, are_instrs,
  //                                HDD);
  //   }
  //   break;
  default:
    fprintf(stderr, "Error: Invalid memory type\n");
    exit(EXIT_FAILURE);
  }
}

char **split_string(const char *str, uint8_t *count) {
  char *str_copy = strdup(str);

  char **result = NULL;
  int words = 0;

  char *token = strtok(str_copy, " \t\n");
  while (token != NULL) {
    result = realloc(result, (words + 1) * sizeof(char *));

    result[words] = strdup(token);
    words++;
    token = strtok(NULL, " \t\n");
  }
  *count = words;

  free(str_copy);

  return result;
}

uint64_t determine_watchpoint_value(char *watchpoint_str) {
  for (int i = 0; i < NUM_REGISTERS; i++) {
    if (strcmp(watchpoint_str, register_code_to_name[i]) == 0) {
      return read_array(regs, i, false);
    }
  }

  char *endptr;
  uint64_t watchpoint_val = strtol(watchpoint_str, &endptr, 10);
  if (*endptr != '\0') {
    const char *str = "Error: Invalid register or number: ";
    const char *str2 = proper_str_cat(str, watchpoint_str);
    const char *str3 = proper_str_cat(str2, "\n");
    fprintf(stderr, "%s", str3);
    invalid_input = true;
    // this value will never be reached
    return UINT64_MAX;
  } else if (watchpoint_val < 0 && watchpoint_val > UINT64_MAX) {
    // TODO:  this makes no sense
    fprintf(stderr, "Error: Number out of range, must be between 0 and "
                    "18446744073709551615\n");
    invalid_input = true;
    // this value will never be reached
    return UINT64_MAX;
  }

  // TODO: later also add in paging physical addresses

  return watchpoint_val;
}

void print_sram_watchpoint(uint64_t sram_watchpoint_x) {
  if (!(sram_watchpoint_x & 0x80000000)) {
    return;
  }
  sram_watchpoint_x = sram_watchpoint_x & 0x7FFFFFFF;
  if (ivt_max_idx != -1) {
    print_file_with_idcs(SRAM, max(0, sram_watchpoint_x - radius),
                         min(sram_watchpoint_x + radius, ivt_max_idx), true,
                         false);
  }
  print_file_with_idcs(
      SRAM, max(ivt_max_idx + 1, sram_watchpoint_x - radius),
      min(sram_watchpoint_x + radius, num_instrs_isrs + num_instrs_prgrm - 1),
      false, true);
  print_file_with_idcs(
      SRAM, max(num_instrs_isrs + num_instrs_prgrm, sram_watchpoint_x - radius),
      min(sram_watchpoint_x + radius, sram_size-1), false, false);
}

void print_uart_meta_data() {
  printf("Current send data: %s\n", current_send_data ? current_send_data : "");
  printf("All send data: %s\n", all_send_data ? all_send_data : "");
  printf("Waiting time sending: ");
  printf("%d\n", sending_waiting_time);
  printf("Waiting time receiving: ");
  printf("%d\n", receiving_waiting_time);
  if (receiving_waiting_time > 0) {
    printf("Current input: %u\n", received_num_part);
  } else {
    printf("Current input:\n");
  }
  printf("Remaining input: ");
  if (read_metadata && input_idx < input_len) {
    for (uint8_t i = input_idx; i < input_len; i++) {
      if (i == input_idx && (int8_t)received_num_idx >= 0) {
        printf("%d(", received_num);
        for (uint8_t j = received_num_idx; j != 0; j--) {
          uint8_t received_num_part =
              (received_num & (0xFF << (j * 8))) >> (j * 8);
          printf("%u ", received_num_part);
        }
        uint8_t received_num_part = received_num & 0xFF;
        printf("%u) ", received_num_part);
      } else {
        printf("%d ", uart_input[i]);
      }
    }
  } else {
    if ((int8_t)received_num_idx >= 0) {
      printf("%d(", received_num);
      for (uint8_t j = received_num_idx; j != 0; j--) {
        uint8_t received_num_part =
            (received_num & (0xFF << (j * 8))) >> (j * 8);
        printf("%u ", received_num_part);
      }
      uint8_t received_num_part = received_num & 0xFF;
      printf("%u)", received_num_part);
    }
  }
  printf("\n");
}

void get_user_input(void) {
  uint8_t count;
  char buffer[26];
  while (true) {
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
      fprintf(stderr, "Error: Reading input not successful\n");
    }
    printf("\033[A\033[K");
    if (invalid_input) {
      printf("\033[A\033[K");
      invalid_input = false;
    }
    fflush(stdout);

    char **split_input = split_string(buffer, &count);
    if (split_input == NULL) {
      continue;
    } else if (strcmp(split_input[0], "n") == 0 && count == 1) {
      return;
    } else if (strcmp(split_input[0], "c") == 0 && count == 1) {
      breakpoint_encountered = false;
      return;
    } else if (strcmp(split_input[0], "s") == 0 && count == 1) {
      if (machine_to_assembly(read_storage(read_array(regs, PC, false)))->op !=
          INT) {
        continue;
      }
      step_into_activated = true;
      return;
    } else if (strcmp(split_input[0], "a") == 0 && count == 3) {
      if (strcmp(split_input[1], "ew") == 0) {
        char *eprom_watchpoint_tmp = eprom_watchpoint;
        eprom_watchpoint = split_input[2];
        if (!draw_tui()) {
          eprom_watchpoint = eprom_watchpoint_tmp;
        }
      } else if (strcmp(split_input[1], "swc") == 0) {
        char *sram_watchpoint_cs_tmp = sram_watchpoint_cs;
        sram_watchpoint_cs = split_input[2];
        if (!draw_tui()) {
          sram_watchpoint_cs = sram_watchpoint_cs_tmp;
        }
      } else if (strcmp(split_input[1], "swd") == 0) {
        char *sram_watchpoint_ds_tmp = sram_watchpoint_ds;
        sram_watchpoint_ds = split_input[2];
        if (!draw_tui()) {
          sram_watchpoint_ds = sram_watchpoint_ds_tmp;
        }
      } else if (strcmp(split_input[1], "sws") == 0) {
        char *sram_watchpoint_stack_tmp = sram_watchpoint_stack;
        sram_watchpoint_stack = split_input[2];
        if (!draw_tui()) {
          sram_watchpoint_stack = sram_watchpoint_stack_tmp;
        }
      } else {
        fprintf(stderr, "Error: Invalid command\n");
        invalid_input = true;
        continue;
      }
      continue;
    } else if (strcmp(split_input[0], "D") == 0) {
#ifdef __linux__
      __asm__("int3"); // ../.gdbinit
#endif
    } else if (strcmp(split_input[0], "q") == 0 && count == 1) {
      exit(EXIT_SUCCESS);
    } else {
      fprintf(stderr, "Error: Invalid command\n");
      invalid_input = true;
      continue;
    }
  }
}

bool draw_tui(void) {
  uint64_t eprom_watchpoint_int = determine_watchpoint_value(eprom_watchpoint);
  uint64_t sram_watchpoint_cs_int =
      determine_watchpoint_value(sram_watchpoint_cs);
  uint64_t sram_watchpoint_ds_int =
      determine_watchpoint_value(sram_watchpoint_ds);
  uint64_t sram_watchpoint_stack_int =
      determine_watchpoint_value(sram_watchpoint_stack);
  if (eprom_watchpoint_int == UINT64_MAX ||
      sram_watchpoint_cs_int == UINT64_MAX ||
      sram_watchpoint_ds_int == UINT64_MAX ||
      sram_watchpoint_stack_int == UINT64_MAX) {
    return false;
  }

  clrscr();
  printf("%s\n", create_heading('-', "Registers", LINEWIDTH));
  print_array_with_idcs(REGS, NUM_REGISTERS, false);

  printf("%s\n", create_heading('-', "EPROM", LINEWIDTH));
  printf("EPROM Watchpoint (ew): %s (%lu)\n", eprom_watchpoint,
         eprom_watchpoint_int);
  print_array_with_idcs_from_to(
      EPROM, max(0, eprom_watchpoint_int - radius),
      min(eprom_watchpoint_int + radius, num_instrs_start_prgrm - 1), true);

  printf("%s\n", create_heading('-', "UART", LINEWIDTH));
  print_array_with_idcs(UART, NUM_UART_ADDRESSES, false);
  print_uart_meta_data();

  // the user shouldn't have to calculate the absolute address for the sram
  sram_watchpoint_cs_int =
      sram_watchpoint_cs_int + (isdigit(sram_watchpoint_cs[0]) ? (1 << 31) : 0);
  sram_watchpoint_ds_int =
      sram_watchpoint_ds_int + (isdigit(sram_watchpoint_ds[0]) ? (1 << 31) : 0);
  sram_watchpoint_stack_int =
      sram_watchpoint_stack_int +
      (isdigit(sram_watchpoint_stack[0]) ? (1 << 31) : 0);
  printf("%s\n", create_heading('-', "SRAM", LINEWIDTH));
  printf("SRAM Watchpoint Codesegment (swc): %s (%lu)\n", sram_watchpoint_cs,
         sram_watchpoint_cs_int);
  print_sram_watchpoint(sram_watchpoint_cs_int);
  printf("SRAM Watchpoint Datasegment (swd): %s (%lu)\n", sram_watchpoint_ds,
         sram_watchpoint_ds_int);
  print_sram_watchpoint(sram_watchpoint_ds_int);
  printf("SRAM Watchpoint Stack (sws): %s (%lu)\n", sram_watchpoint_stack,
         sram_watchpoint_stack_int);
  print_sram_watchpoint(sram_watchpoint_stack_int);

  printf("%s\n", create_heading('=', "Possible actions", LINEWIDTH));
  printf("(n)ext instruction, (c)ontinue to breakpoint, (q)uit\n");
  if (extended_features) {
    printf("(s)tep into isr, (a)ssign watchpoint reg or addr\n");
  } else {
    printf("(a)ssign watchpoint reg or addr\n");
  }

  // print_file_idcs(hdd, max(0, hdd_view_pos - radius),
  //                 min(hdd_view_pos + radius, hdd_size-1), false);
  return true;
}
