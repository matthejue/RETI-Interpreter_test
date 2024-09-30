#include "../include/daemon.h"
#include "../include/globals.h"
#include "../include/interpr.h"
#include "../include/reti.h"
#include "../include/utils.h"
#include <stdbool.h>
#include <stdlib.h>

// TODO: Problem, dass immediates sign extended werden, aber bitweise xor, and
// und or auf das nicht sign extendete mit 0en drangefügt angewandt werden
// TODO: Alernative Lösung ohne sign extension mit:
// typedef struct {
//     uint32_t value : 22;
// } uint22_t;
void interpr_instr(Instruction *assembly_instr) {
  switch (assembly_instr->op) {
  case ADDI:
    write_array(regs, assembly_instr->opd1,
                (int32_t)read_array(regs, assembly_instr->opd1) +
                    (int32_t)assembly_instr->opd2);
    break;
  case SUBI:
    write_array(regs, assembly_instr->opd1,
                (int32_t)read_array(regs, assembly_instr->opd1) -
                    (int32_t)assembly_instr->opd2);
    break;
  case MULTI:
    write_array(regs, assembly_instr->opd1,
                (int32_t)read_array(regs, assembly_instr->opd1) *
                    (int32_t)assembly_instr->opd2);
    break;
  case DIVI:
    write_array(regs, assembly_instr->opd1,
                (int32_t)read_array(regs, assembly_instr->opd1) /
                    (int32_t)assembly_instr->opd2);
    break;
  case MODI:
    write_array(regs, assembly_instr->opd1,
                mod((int32_t)read_array(regs, assembly_instr->opd1),
                    (int32_t)assembly_instr->opd2));
    break;
  case OPLUSI:
    write_array(regs, assembly_instr->opd1,
                read_array(regs, assembly_instr->opd1) ^
                    (assembly_instr->opd2 & IMMEDIATE_MASK));
    break;
  case ORI:
    write_array(regs, assembly_instr->opd1,
                read_array(regs, assembly_instr->opd1) |
                    (assembly_instr->opd2 & IMMEDIATE_MASK));
    break;
  case ANDI:
    write_array(regs, assembly_instr->opd1,
                read_array(regs, assembly_instr->opd1) &
                    (assembly_instr->opd2 & IMMEDIATE_MASK));
    break;
  case ADDR:
    write_array(regs, assembly_instr->opd1,
                (int32_t)read_array(regs, assembly_instr->opd1) +
                    (int32_t)read_array(regs, assembly_instr->opd2));
    break;
  case SUBR:
    write_array(regs, assembly_instr->opd1,
                (int32_t)read_array(regs, assembly_instr->opd1) -
                    (int32_t)read_array(regs, assembly_instr->opd2));
    break;
  case MULTR:
    write_array(regs, assembly_instr->opd1,
                (int32_t)read_array(regs, assembly_instr->opd1) *
                    (int32_t)read_array(regs, assembly_instr->opd2));
    break;
  case DIVR:
    write_array(regs, assembly_instr->opd1,
                (int32_t)read_array(regs, assembly_instr->opd1) /
                    (int32_t)read_array(regs, assembly_instr->opd2));
    break;
  case MODR:
    write_array(regs, assembly_instr->opd1,
                mod((int32_t)read_array(regs, assembly_instr->opd1),
                    (int32_t)read_array(regs, assembly_instr->opd2)));
    break;
  case OPLUSR:
    write_array(regs, assembly_instr->opd1,
                read_array(regs, assembly_instr->opd1) ^
                    read_array(regs, assembly_instr->opd2));
    break;
  case ORR:
    write_array(regs, assembly_instr->opd1,
                read_array(regs, assembly_instr->opd1) |
                    read_array(regs, assembly_instr->opd2));
    break;
  case ANDR:
    write_array(regs, assembly_instr->opd1,
                read_array(regs, assembly_instr->opd1) &
                    read_array(regs, assembly_instr->opd2));
    break;
  case ADDM:
    write_array(regs, assembly_instr->opd1,
                (int32_t)read_array(regs, assembly_instr->opd1) +
                    (int32_t)read_storage_ds_fill(assembly_instr->opd2));
    break;
  case SUBM:
    write_array(regs, assembly_instr->opd1,
                (int32_t)read_array(regs, assembly_instr->opd1) -
                    (int32_t)read_storage_ds_fill(assembly_instr->opd2));
    break;
  case MULTM:
    write_array(regs, assembly_instr->opd1,
                (int32_t)read_array(regs, assembly_instr->opd1) *
                    (int32_t)read_storage_ds_fill(assembly_instr->opd2));
    break;
  case DIVM:
    write_array(regs, assembly_instr->opd1,
                (int32_t)read_array(regs, assembly_instr->opd1) /
                    (int32_t)read_storage_ds_fill(assembly_instr->opd2));
    break;
  case MODM:
    write_array(regs, assembly_instr->opd1,
                mod((int32_t)read_array(regs, assembly_instr->opd1),
                    (int32_t)read_storage_ds_fill(assembly_instr->opd2)));
    break;
  case OPLUSM:
    write_array(regs, assembly_instr->opd1,
                read_array(regs, assembly_instr->opd1) ^
                    read_storage_ds_fill(assembly_instr->opd2));
    break;
  case ORM:
    write_array(regs, assembly_instr->opd1,
                read_array(regs, assembly_instr->opd1) |
                    read_storage_ds_fill(assembly_instr->opd2));
    break;
  case ANDM:
    write_array(regs, assembly_instr->opd1,
                read_array(regs, assembly_instr->opd1) &
                    read_storage_ds_fill(assembly_instr->opd2));
    break;
  case LOAD:
    write_array(regs, assembly_instr->opd1,
                read_storage_ds_fill(assembly_instr->opd2));
    break;
  case LOADIN:
    write_array(regs, assembly_instr->opd2,
                read_storage(read_array(regs, assembly_instr->opd1) +
                             (int32_t)assembly_instr->opd3));
    break;
  case LOADI:
    // TODO: Das mit der Maske entfernen, falls i auch signed sein darf
    write_array(regs, assembly_instr->opd1,
                assembly_instr->opd2 & IMMEDIATE_MASK);
    break;
  case STORE:
    write_storage_ds_fill(assembly_instr->opd2,
                          read_array(regs, assembly_instr->opd1));
    break;
  case STOREIN:
    write_storage(read_array(regs, assembly_instr->opd1) +
                      (int32_t)assembly_instr->opd3,
                  read_array(regs, assembly_instr->opd2));
    break;
  case MOVE:
    write_array(regs, assembly_instr->opd2,
                read_array(regs, assembly_instr->opd1));
    break;
  case NOP:
    break;
  case INT:
    write_array(regs, SP, read_array(regs, SP) - 1);
    write_storage(read_array(regs, SP) + 1, read_array(regs, PC));
    write_array(regs, PC, read_storage_ds_fill(assembly_instr->opd1));
    break;
  case RTI:
    write_array(regs, PC, read_storage(read_array(regs, SP) + 1));
    write_array(regs, SP, read_array(regs, SP) + 1);
    break;
  case JUMPGT:
    if (read_array(regs, ACC) > 0) {
      write_array(regs, PC,
                  read_array(regs, PC) + (int32_t)assembly_instr->opd1);
      goto no_pc_increase;
    }
    break;
  case JUMPEQ:
    if (read_array(regs, ACC) == 0) {
      write_array(regs, PC,
                  read_array(regs, PC) + (int32_t)assembly_instr->opd1);
      goto no_pc_increase;
    }
    break;
  case JUMPGE:
    if (read_array(regs, ACC) >= 0) {
      write_array(regs, PC,
                  read_array(regs, PC) + (int32_t)assembly_instr->opd1);
      goto no_pc_increase;
    }
    break;
  case JUMPLT:
    if (read_array(regs, ACC) < 0) {
      write_array(regs, PC,
                  read_array(regs, PC) + (int32_t)assembly_instr->opd1);
      goto no_pc_increase;
    }
    break;
  case JUMPNE:
    if (read_array(regs, ACC) != 0) {
      write_array(regs, PC,
                  read_array(regs, PC) + (int32_t)assembly_instr->opd1);
      goto no_pc_increase;
    }
    break;
  case JUMPLE:
    if (read_array(regs, ACC) <= 0) {
      write_array(regs, PC,
                  read_array(regs, PC) + (int32_t)assembly_instr->opd1);
      goto no_pc_increase;
    }
    break;
  case JUMP:
    write_array(regs, PC, read_array(regs, PC) + (int32_t)assembly_instr->opd1);
    goto no_pc_increase;
  default:
    perror("Error a instruction with this opcode doesn't exist yet");
    exit(EXIT_FAILURE);
  }
  write_array(regs, PC, read_array(regs, PC) + 1);
no_pc_increase:;
}

void interpr_prgrm() {
  while (true) {
    uint32_t machine_instr = read_storage(read_array(regs, PC));
    Instruction *assembly_instr = machine_to_assembly(machine_instr);

    if (daemon_mode) {
      cont();
    }

    if (assembly_instr->op == JUMP && assembly_instr->opd1 == 0) {
      free(assembly_instr);
      break;
    } else {
      interpr_instr(assembly_instr);
      free(assembly_instr);
    }
  }
}
