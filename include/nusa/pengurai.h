// Copyright 2026 Pengembang Bahasa Pemrograman Nusa
// SPDX-License-Identifier: Apache-2.0

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
  PSA_KATA_KUNCI,         // eks
  PSA_KATA_KUNCI_PUBLIK,  // pub
  PSA_TIPE_DATA_BILANGAN, // b32
  PSA_TIPE_DATA_UNTAIAN,  // unt
  PSA_BLOK_OS_ARSITEKTUR, // windows, linux, intel_32, intel_64 { ... }
  PSA_SO_WINDOWS,
  PSA_SO_LINUX,
  PSA_ARS_INTEL_32,
  PSA_ARS_INTEL_64,
  PSA_MUAT,
  PSA_TITIK,
} TipePSA;

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

PSA *urai(const char *isi, const char *jalur_berkas);

const char *pesan_urai();

void cetak_psa(const PSA *node, int indent);

void bersihkan_psa(PSA *node);

#endif