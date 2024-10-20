#include "../include/error.h"
#include "../include/globals.h"
#include "../include/interpr.h"
#include "../include/parse_args.h"
#include "../include/parse_instrs.h"
#include "../include/reti.h"
#include "../include/utils.h"
#include "../include/test_mode.h"
#include "../include/uart.h"
#include <string.h>

int main(int argc, char *argv[]) {
  parse_args(argc, argv);
  if (verbose) {
    print_args();
  }
  if (test_mode) {
    uart_input = extract_comment_metadata(sram_prgrm_path, &input_len);
  }

  init_reti();

  if (strcmp(isrs_prgrm_path, "") != 0) {
    error_context.filename = isrs_prgrm_path;
    parse_and_load_program(get_prgrm_content(isrs_prgrm_path), ISR_PRGRMS);
  }

  error_context.filename = sram_prgrm_path;
  parse_and_load_program(get_prgrm_content(sram_prgrm_path), SRAM_PRGRM);

  if (strcmp(eprom_prgrm_path, "") != 0) {
    error_context.filename = eprom_prgrm_path;
    parse_and_load_program(get_prgrm_content(eprom_prgrm_path),
                           EPROM_START_PRGRM);
  } else {
    load_adjusted_eprom_prgrm();
  }

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
