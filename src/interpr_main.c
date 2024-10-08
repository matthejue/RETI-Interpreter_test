// #include <stdio.h>
#include "../include/globals.h"
#include "../include/interpr.h"
#include "../include/parse_args.h"
#include "../include/parse_instrs.h"
#include "../include/reti.h"
#include "../include/utils.h"
#include <string.h>

int main(int argc, char *argv[]) {
  parse_args(argc, argv);
  print_args();

  init_reti();

  if (strcmp(eprom_prgrm_path, "") != 0) {
    parse_and_load_program(get_prgrm_content(eprom_prgrm_path), EPROM_START_PRGRM);
  } else {
    load_adjusted_eprom_prgrm();
  }
  if (strcmp(isrs_prgrm_path, "") != 0) {
    parse_and_load_program(get_prgrm_content(isrs_prgrm_path), ISR_PRGRMS);
  } else {
    load_isrs();
  }
  parse_and_load_program(get_prgrm_content(sram_prgrm_path), SRAM_PRGRM);

  interpr_prgrm();

  fin_reti();
  return 0;
}

// TODO:
// - cli options:
//   - RAM size
//   - page size
// - reti, eprom, uart, sram, sata
//   - Befehle am Anfang reinladen ins CS
// - paging table generieren
// - eprom, regs und uart keine Dateien, nur sram und hdd
// - Startprogram bei der Kompilierung schon vorher erstellen
// - Kommentare supporten
// - INT und RETI
// - Test machen, der alle Instructions einmal ausführt
// - Memory Map über DS Register steuerbar machen
// - Error cases, division by zero usw. hinzufügen
// - eigene utils.c und utils.h mit mod, max und min und sign extension
// - vergessen, dass längster 22 bit Integer -4194304 bis 9 characters braucht
// TODO: Tobias fragen wegen strings und readonly data segment
// TODO: Tobias fragen, ob padding und alignment für structs notwendig
// TODO: Tobias: memory contant that corresponds to utf8 / ascii letter print as
// ascii or just number?
// TODO: Tobias: Was wenn man negative 22 Bit 2er Komplementzahl bitwise, xor,
// and oder or nimmt?
