#ifndef NUSA_PENGURAI_H
#define NUSA_PENGURAI_H

typedef enum {
  PSA_PROGRAM,
  PSA_PANGGILAN,
  PSA_PENGENAL,
  PSA_NILAI_UNTAIAN,
  PSA_KATA_KUNCI,
} TipePSA;

typedef struct PSA PSA;
struct PSA {
  TipePSA tipe;
  const char *teks;
  int panjang;
  PSA **anak;
  int jumlah_anak;
};

PSA *urai(const char *isi);

const char *pesan_urai();

void cetak_psa(const PSA *node, int indent);

void bersihkan_psa(PSA *node);

#endif