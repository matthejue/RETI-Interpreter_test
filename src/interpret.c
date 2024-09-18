#include "../include/interpret.h"
#include "../include/reti.h"
#include <stdlib.h>
#include <stdbool.h>

void interpr_instruction(Instruction *assembly_instr) {
  switch (assembly_instr->op) {
  case ADDI:
    write_storage(regs, assembly_instr->opd1,
                  (int32_t)read_storage(regs, assembly_instr->opd1) +
                      (int32_t)assembly_instr->opd2);
    break;
  case SUBI:
    write_storage(regs, assembly_instr->opd1,
                  read_storage(regs, assembly_instr->opd1) -
                      (int32_t)assembly_instr->opd2);
    break;
  case MULTI:
    write_storage(regs, assembly_instr->opd1,
                  (int32_t)read_storage(regs, assembly_instr->opd1) *
                      (int32_t)assembly_instr->opd2);
    break;
  case DIVI:
    write_storage(regs, assembly_instr->opd1,
                  (int32_t)read_storage(regs, assembly_instr->opd1) /
                      (int32_t)assembly_instr->opd2);
    break;
  case MODI:
    write_storage(regs, assembly_instr->opd1,
                  (int32_t)read_storage(regs, assembly_instr->opd1) %
                      (int32_t)assembly_instr->opd2);
    break;
  case OPLUSI:
    write_storage(regs, assembly_instr->opd1,
                  read_storage(regs, assembly_instr->opd1) ^
                      assembly_instr->opd2);
    break;
  case ORI:
    write_storage(regs, assembly_instr->opd1,
                  read_storage(regs, assembly_instr->opd1) |
                      assembly_instr->opd2);
    break;
  case ANDI:
    write_storage(regs, assembly_instr->opd1,
                  read_storage(regs, assembly_instr->opd1) &
                      assembly_instr->opd2);
    break;
  case ADDR:
    write_storage(regs, assembly_instr->opd1,
                  (int32_t)read_storage(regs, assembly_instr->opd1) +
                      (int32_t)read_storage(regs, assembly_instr->opd2));
    break;
  case SUBR:
    write_storage(regs, assembly_instr->opd1,
                  (int32_t)read_storage(regs, assembly_instr->opd1) -
                      (int32_t)read_storage(regs, assembly_instr->opd2));
    break;
  case MULTR:
    write_storage(regs, assembly_instr->opd1,
                  (int32_t)read_storage(regs, assembly_instr->opd1) *
                      (int32_t)read_storage(regs, assembly_instr->opd2));
    break;
  case DIVR:
    write_storage(regs, assembly_instr->opd1,
                  (int32_t)read_storage(regs, assembly_instr->opd1) /
                      (int32_t)read_storage(regs, assembly_instr->opd2));
    break;
  case MODR:
    write_storage(regs, assembly_instr->opd1,
                  (int32_t)read_storage(regs, assembly_instr->opd1) %
                      (int32_t)read_storage(regs, assembly_instr->opd2));
    break;
  case OPLUSR:
    write_storage(regs, assembly_instr->opd1,
                  read_storage(regs, assembly_instr->opd1) ^
                      read_storage(regs, assembly_instr->opd2));
    break;
  case ORR:
    write_storage(regs, assembly_instr->opd1,
                  read_storage(regs, assembly_instr->opd1) |
                      read_storage(regs, assembly_instr->opd2));
    break;
  case ANDR:
    write_storage(regs, assembly_instr->opd1,
                  read_storage(regs, assembly_instr->opd1) &
                      read_storage(regs, assembly_instr->opd2));
    break;
  case ADDM:
    write_storage(regs, assembly_instr->opd1,
                  (int32_t)read_storage(regs, assembly_instr->opd1) +
                      (int32_t)read_file(sram, assembly_instr->opd2));
    break;
  case SUBM:
    write_storage(regs, assembly_instr->opd1,
                  (int32_t)read_storage(regs, assembly_instr->opd1) -
                      (int32_t)read_file(sram, assembly_instr->opd2));
    break;
  case MULTM:
    write_storage(regs, assembly_instr->opd1,
                  (int32_t)read_storage(regs, assembly_instr->opd1) *
                      (int32_t)read_file(sram, assembly_instr->opd2));
    break;
  case DIVM:
    write_storage(regs, assembly_instr->opd1,
                  (int32_t)read_storage(regs, assembly_instr->opd1) /
                      (int32_t)read_file(sram, assembly_instr->opd2));
    break;
  case MODM:
    write_storage(regs, assembly_instr->opd1,
                  (int32_t)read_storage(regs, assembly_instr->opd1) %
                      (int32_t)read_file(sram, assembly_instr->opd2));
    break;
  case OPLUSM:
    write_storage(regs, assembly_instr->opd1,
                  read_storage(regs, assembly_instr->opd1) ^
                      read_file(sram, assembly_instr->opd2));
    break;
  case ORM:
    write_storage(regs, assembly_instr->opd1,
                  read_storage(regs, assembly_instr->opd1) |
                      read_file(sram, assembly_instr->opd2));
    break;
  case ANDM:
    write_storage(regs, assembly_instr->opd1,
                  read_storage(regs, assembly_instr->opd1) &
                      read_file(sram, assembly_instr->opd2));
    break;
  case LOAD:
    write_storage(regs, assembly_instr->opd1,
                  read_file(sram, assembly_instr->opd2));
    break;
  case LOADIN:
    write_storage(regs, assembly_instr->opd2,
                  read_file(sram, read_storage(regs, assembly_instr->opd1) +
                                      (int32_t)assembly_instr->opd3));
    break;
  case LOADI:
    write_storage(regs, assembly_instr->opd1, assembly_instr->opd2);
    break;
  case STORE:
    write_file(sram, assembly_instr->opd2,
               read_storage(regs, assembly_instr->opd1));
    break;
  case STOREIN:
    write_file(sram,
               read_storage(regs, assembly_instr->opd1) +
                   (int32_t)assembly_instr->opd3,
               read_storage(regs, assembly_instr->opd2));
    break;
  case MOVE:
    write_storage(regs, assembly_instr->opd2,
                  read_storage(regs, assembly_instr->opd1));
    break;
  case NOP:
    break;
  case INT:
    // TODO:
    break;
  case RTI:
    // TODO:
    break;
  case JUMPGT:
    if (ACC > 0) {
      write_storage(regs, PC,
                    read_storage(regs, PC) + (int32_t)assembly_instr->opd1 - 1);
    }
    break;
  case JUMPEQ:
    if (ACC == 0) {
      write_storage(regs, PC,
                    read_storage(regs, PC) + (int32_t)assembly_instr->opd1 - 1);
    }
    break;
  case JUMPGE:
    if (ACC >= 0) {
      write_storage(regs, PC,
                    read_storage(regs, PC) + (int32_t)assembly_instr->opd1 - 1);
    }
    break;
  case JUMPLT:
    if (ACC < 0) {
      write_storage(regs, PC,
                    read_storage(regs, PC) + (int32_t)assembly_instr->opd1 - 1);
    }
    break;
  case JUMPNE:
    if (ACC != 0) {
      write_storage(regs, PC,
                    read_storage(regs, PC) + (int32_t)assembly_instr->opd1 - 1);
    }
    break;
  case JUMPLE:
    if (ACC <= 0) {
      write_storage(regs, PC,
                    read_storage(regs, PC) + (int32_t)assembly_instr->opd1 - 1);
    }
    break;
  case JUMP:
    write_storage(regs, PC,
                  read_storage(regs, PC) + (int32_t)assembly_instr->opd1 - 1);
    break;
  default:
    perror("Error a instruction with this opcode doesn't exist yet");
    exit(EXIT_FAILURE);
  }
  write_storage(regs, PC, read_storage(regs, PC) + 1);
}

void interpr_program() {
  while (true) {
    uint32_t machine_instr = read_file(sram, read_storage(regs, PC));
    Instruction *assembly_instr = machine_to_assembly(machine_instr);

    if (assembly_instr->op == JUMP && assembly_instr->opd1 == 0) {
      free(assembly_instr);
      break;
    } else {
      interpr_instruction(assembly_instr);
      free(assembly_instr);
    }
  }
}
