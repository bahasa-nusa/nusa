#ifndef NUSA_RA_H
#define NUSA_RA_H

#include "nusa/pengurai.h"

typedef enum {
  RA_FUNGSI,
  RA_PANGGIL,
} TipeRA;

typedef enum {
  RA_TANPA_TIPE,
  RA_BILANGAN,
  RA_UNTAIAN,
} TipeNilaiRA;

typedef struct InstruksiRA {
  TipeRA tipe;
  char *nama;
  char **nilai;
  TipeNilaiRA *tipe_nilai;
  int jumlah;
  bool eks;
  bool pub;
  char *modul;
  struct InstruksiRA *badan;
  struct InstruksiRA *next;
} InstruksiRA;

InstruksiRA *bangkitkan_ra(const PSA *akar);
InstruksiRA *tambah_impor_ra(InstruksiRA *daftar, const InstruksiRA *sumber);
void cetak_ra_permodul(const InstruksiRA *daftar);
void bersihkan_ra(InstruksiRA *daftar);

#endif