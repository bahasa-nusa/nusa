#ifndef NUSA_PENGURAI_H
#define NUSA_PENGURAI_H

#include <stdbool.h>

typedef enum {
  PSA_PROGRAM,
  PSA_BERKAS,
  PSA_PANGGILAN,
  PSA_PENGENAL,
  PSA_NILAI_UNTAIAN,
  PSA_KATA_KUNCI,
} TipePSA;

typedef struct PSA PSA;
struct PSA {
  TipePSA tipe;
  char *teks;
  int panjang;
  char *jalur;
  bool titik_masuk;
  PSA **anak;
  int jumlah_anak;
};

PSA *urai(const char *isi, const char *jalur_berkas);

const char *pesan_urai();

void cetak_psa(const PSA *node, int indent);

void bersihkan_psa(PSA *node);

#endif