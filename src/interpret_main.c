// #include <stdio.h>
#include "../include/globals.h"
#include "../include/parse_args.h"
#include "../include/parse_instrs.h"
#include "../include/reti.h"
#include "../include/interpret.h"
#include <stdlib.h>

int main(int argc, char *argv[]) {
  char *input = NULL;

  parse_arguments(argc, argv, &input);

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
