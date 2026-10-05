#include "nusa/berkas.h"

#include <stdio.h>
#include <stdlib.h>

const char *baca_berkas(const char *nama_berkas) {
  FILE *berkas = fopen(nama_berkas, "rb");
  if (!berkas) {
    return NULL;
  }

  if (fseek(berkas, 0, SEEK_END) != 0) {
    fclose(berkas);
    return NULL;
  }

  long ukuran = ftell(berkas);

  if (ukuran < 0) {
    fclose(berkas);
    return NULL;
  }

  rewind(berkas);

  char *isi = (char *)malloc((size_t)ukuran + 1);
  if (!isi) {
    fclose(berkas);
    return NULL;
  }

  size_t terbaca = ukuran ? fread(isi, 1, (size_t)ukuran, berkas) : 0;

  fclose(berkas);

  isi[terbaca] = '\0';
  return isi;
}

void bersihkan_berkas(const char *isi_berkas) { free((void *)isi_berkas); }