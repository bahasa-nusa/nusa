#ifndef NUSA_PENGURAI_H
#define NUSA_PENGURAI_H

#include <stdbool.h>

typedef enum {
  PSA_PROGRAM,
  PSA_BERKAS,
  PSA_PANGGILAN,
  PSA_DEKLARASI,
  PSA_PENGENAL,
  PSA_NILAI_UNTAIAN,
  PSA_NILAI_BILANGAN,
  PSA_KATA_KUNCI,
  PSA_KATA_KUNCI_PUBLIK,
  PSA_TIPE_DATA_BILANGAN,
  PSA_TIPE_DATA_UNTAIAN,
  PSA_MOD,
  PSA_TITIK,
} TipePSA;

#define PSA_MUAT PSA_MOD

typedef struct PSA PSA;
struct PSA {
  TipePSA tipe;
  char *teks;
  int panjang;
  char *jalur;
  char *modul;
  bool titik_masuk;
  PSA **anak;
  int jumlah_anak;
};

PSA *urai_banyak_berkas(const char **daftar_jalur, int jumlah_berkas);

PSA *urai(const char *isi, const char *jalur_berkas);

const char *pesan_urai(void);

void cetak_psa(const PSA *node, int indent);

void bersihkan_psa(PSA *node);

#endif