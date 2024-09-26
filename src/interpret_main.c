// #include <stdio.h>
#include "../include/globals.h"
#include "../include/parse_args.h"
#include "../include/parse_instrs.h"
#include "../include/reti.h"
#include "../include/interpret.h"
#include <stdlib.h>

int main(int argc, char *argv[]) {
  char *input = NULL;

  parse_args(argc, argv, &input);

  printf("RAM Size: %d\n", sram_size);
  printf("Page Size: %d\n", page_size);
  printf("HDD Size: %d\n", hdd_size);
  printf("Input: %s\n", input);

  init_reti();

  parse_and_load_program(input);

  interpr_program();

  fin_reti();

  free(input);
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
// TODO: Tobias: memory contant that corresponds to utf8 / ascii letter print as ascii or just number?
// TODO: Tobias: Was wenn man negative 22 Bit 2er Komplementzahl bitwise, xor, and oder or nimmt?
