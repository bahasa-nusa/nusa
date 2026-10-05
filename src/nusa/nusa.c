#include <stdio.h>

#include "nusa/berkas.h"
#include "nusa/penolek.h"
#include "nusa/urai_arg.h"

void cetak_info() {
  printf("Penggunaan: nusa [argumen] <berkas>\n\n");
  printf("Opsi:\n");
  printf("-v, --versi     Untuk melihat versi.\n");
  printf("-i, --info      Untuk melihat informasi penggunaan.\n");
}

int main(int argc, char **argv) {
  Arg arg = urai_arg(argc, argv);

  if (arg.versi) {
    printf("nusa v0.0.0\n");
    return 0;
  }

  if (arg.info) {
    cetak_info();
    return 0;
  }

  if (arg.input_file) {
    const char *isi_berkas = baca_berkas(arg.input_file);
    if (!isi_berkas) {
      printf("Gagal baca: %s\n", arg.input_file);
      return 1;
    }

    const char *ptr = isi_berkas;
    Tolek tolek;
    while ((ptr = penolek(ptr, &tolek)) && tolek.tipe != TIPE_TOLEK_AKHIR) {
      printf("%22.*s | %s\n", tolek.panjang, tolek.teks,
             nama_tolek(tolek.tipe));
    }

    printf("%22.*s | %s\n", tolek.panjang, tolek.teks, nama_tolek(tolek.tipe));

    bersihkan_berkas(isi_berkas);
  } else {
    printf("Argumen tidak valid.\n");
    cetak_info();
  }

  return 0;
}