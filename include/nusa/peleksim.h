// Copyright 2026 Pengembang Bahasa Pemrograman Nusa
// SPDX-License-Identifier: Apache-2.0

#ifndef NUSA_PELEKSIM_H
#define NUSA_PELEKSIM_H

typedef enum {
  TIPE_LEKSIM_AKHIR,
  TIPE_LEKSIM_TIDAK_DIKETAHUI,
  TIPE_LEKSIM_KOMENTAR,
  TIPE_LEKSIM_NILAI_UNTAIAN,
  TIPE_LEKSIM_NILAI_BILANGAN,
  TIPE_LEKSIM_PENGENAL,
  TIPE_LEKSIM_KURUNG_BULAT_BUKA,
  TIPE_LEKSIM_KURUNG_BULAT_TUTUP,
  TIPE_LEKSIM_KATA_KUNCI_EKSTERNAL, // eks
  TIPE_LEKSIM_KATA_KUNCI_PUBLIK,    // pub
  TIPE_LEKSIM_TIPE_DATA_BILANGAN,   // b32
  TIPE_LEKSIM_TIPE_DATA_UNTAIAN,    // unt
  TIPE_LEKSIM_KOMA,                 // ,
  TIPE_LEKSIM_KURUNG_KURAWAT_BUKA,  // {
  TIPE_LEKSIM_KURUNG_KURAWAT_TUTUP, // }
  TIPE_LEKSIM_OPERASI_ISI,          // =
  TIPE_LEKSIM_TIPE_DATA_MUAT,       // muat
  TIPE_LEKSIM_TITIK,                // .
} TipeLeksim;

typedef struct {
  TipeLeksim tipe;
  const char *teks;
  int panjang;
} Leksim;

const char *peleksim(const char *isi, Leksim *hasil);
const char *nama_leksim(TipeLeksim tipe);

#endif
