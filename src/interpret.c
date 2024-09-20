#include "../include/interpret.h"
#include "../include/reti.h"
#include "../include/globals.h"
#include "../include/daemon.h"
#include <stdbool.h>
#include <stdlib.h>

// r = a % m <-> r + m * q = a + m * p <-> r = a - m * q (q is biggest q such that q*m <= a) = a − m * (a / m)
// 0 <= r < m, a,q € Z, m € N/0 (normal C / hardware implementation ignores 0 <= r < m)
// 7 % 3 = 7 - 3 * (7 / 3) = 1
// -7 % 3 = -7 - 3 * (-7 / 3) = -1, but r has to be positive, get next congruent element of congruence class by -1 + 3 = 2 (one can easily get negative or positive r with -m < r < m by just nr = r - m and also r = nr + m)
// irrelevant cases where on ignores m € N/0:
// 7 % -3 = 7 - (-3) * (7 / -3) = 1 (if we ignore m € N/0), but that is only for truncated division, for floor it is 7 - (-3) * (7 / -3) = 7 - (-3) * -3 = 7 - 9 = -2 (if we ignore 0 <= r < m and say -m < r < m)
// -7 % -3 = -7 - (-3) * (-7 / -3) = -1 (if we ignore m € N/0 and 0 =< r < m and thus -m < r < m), for floor it is the same: -7 - (-3) * (-7 / -3) = -7 - (-3) * 2 = -7 - (-6) = -1, -1 + 3 = 2 (if don't want to also ignore 0 =< r < m)

uint32_t mod(int32_t a, int32_t b) {
  // r = a − m x trunc(a/m)
  int32_t result = a % b;
  if (result < 0) {
    result += b;
  }
  return result;
}

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
                  mod((int32_t)read_storage(regs, assembly_instr->opd1),
                      (int32_t)assembly_instr->opd2));
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
                  mod((int32_t)read_storage(regs, assembly_instr->opd1),
                      (int32_t)read_storage(regs, assembly_instr->opd2)));
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
                  mod((int32_t)read_storage(regs, assembly_instr->opd1),
                      (int32_t)read_file(sram, assembly_instr->opd2)));
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
    write_storage(regs, SP, read_storage(regs, SP) - 1);
    write_file(sram, read_storage(regs, SP) + 1, read_storage(regs, PC));
    write_storage(regs, PC, read_file(sram, assembly_instr->opd1));
    break;
  case RTI:
    write_storage(regs, PC, read_file(sram, read_storage(regs, SP) + 1));
    write_storage(regs, SP, read_storage(regs, SP) + 1);
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

    if (daemon_mode) {
      cont();
    }

    if (assembly_instr->op == JUMP && assembly_instr->opd1 == 0) {
      free(assembly_instr);
      break;
    } else {
      interpr_instruction(assembly_instr);
      free(assembly_instr);
    }
  }
}
