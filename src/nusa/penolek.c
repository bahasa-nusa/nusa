// Copyright 2026 Pengembang Bahasa Pemrograman Nusa
// SPDX-License-Identifier: Apache-2.0

#include "nusa/penolek.h"
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

static bool batas_kata_kunci(const char *p, int panjang) {
  char c = p[panjang];
  return c == '\0' || isspace((unsigned char)c) || c == '(' || c == ')' ||
         c == '{' || c == '}' || c == ',';
}

const char *nama_tolek(TipeTolek tipe) {
  switch (tipe) {
  case TIPE_TOLEK_AKHIR:
    return "AKHIR";
  case TIPE_TOLEK_KOMENTAR:
    return "COMMENT";
  case TIPE_TOLEK_NILAI_UNTAIAN:
    return "NILAI UNTAIAN";
  case TIPE_TOLEK_NILAI_BILANGAN:
    return "NILAI BILANGAN";
  case TIPE_TOLEK_PENGENAL:
    return "PENGENAL";
  case TIPE_TOLEK_KURUNG_BULAT_BUKA:
    return "KURUNG BULAT BUKA";
  case TIPE_TOLEK_KURUNG_BULAT_TUTUP:
    return "KURUNG BULAT TUTUP";
  case TIPE_TOLEK_KATA_KUNCI_EKSTERNAL:
    return "KATA KUNCI EKSTERNAL";
  case TIPE_TOLEK_KATA_KUNCI_PUBLIK:
    return "KATA KUNCI PUBLIK";
  case TIPE_TOLEK_TIPE_DATA_BILANGAN:
    return "TIPE DATA B32";
  case TIPE_TOLEK_TIPE_DATA_UNTAIAN:
    return "TIPE DATA UNTAIAN";
  case TIPE_TOLEK_SO_WINDOWS:
    return "SISTEM OPERASI WINDOWS";
  case TIPE_TOLEK_SO_LINUX:
    return "SISTEM OPERASI LINUX";
  case TIPE_TOLEK_ARS_INTEL_32:
    return "ARSITEKTUR INTEL 32";
  case TIPE_TOLEK_ARS_INTEL_64:
    return "ARSITEKTUR INTEL 64";
  case TIPE_TOLEK_KOMA:
    return "KOMA";
  case TIPE_TOLEK_KURUNG_KURAWAT_BUKA:
    return "KURUNG KURAWAT BUKA";
  case TIPE_TOLEK_KURUNG_KURAWAT_TUTUP:
    return "KURUNG KURAWAT TUTUP";
  case TIPE_TOLEK_OPERASI_ISI:
    return "OPERASI ISI";
  case TIPE_TOLEK_TIPE_DATA_MUAT:
    return "TIPE DATA MUAT";
  case TIPE_TOLEK_TITIK:
    return "TITIK";
  default:
    return "TIDAK DIKETAHUI";
  }
}

const char *penolek(const char *isi, Tolek *hasil) {
  while (*isi && isspace((unsigned char)*isi) && *isi != '\n')
    isi++;

  if (!*isi) {
    hasil->tipe = TIPE_TOLEK_AKHIR;
    hasil->teks = isi;
    hasil->panjang = 0;
    return isi;
  }

  if (*isi == '\n') {
    isi++;
    return penolek(isi, hasil);
  }

  if (*isi == '#') {
    const char *awal = isi;
    while (*isi && *isi != '\n' && *isi != '\r')
      isi++;
    hasil->tipe = TIPE_TOLEK_KOMENTAR;
    hasil->teks = awal;
    hasil->panjang = (int)(isi - awal);
    return isi;
  }

  if (*isi == '\'' || *isi == '\"') {
    char quote = *isi;
    const char *awal = isi++;
    while (*isi && *isi != quote && *isi != '\n')
      isi++;
    if (*isi == quote)
      isi++;

    hasil->tipe = TIPE_TOLEK_NILAI_UNTAIAN;
    hasil->teks = awal;
    hasil->panjang = (int)(isi - awal);
    return isi;
  }

  if (isdigit((unsigned char)*isi)) {
    const char *awal = isi;
    while (*isi && isdigit((unsigned char)*isi))
      isi++;
    hasil->tipe = TIPE_TOLEK_NILAI_BILANGAN;
    hasil->teks = awal;
    hasil->panjang = (int)(isi - awal);
    return isi;
  }

  if (isalpha((unsigned char)*isi)) {
    const char *awal = isi;

    if (strncmp(isi, "eks", 3) == 0 &&
        (isi[3] == '\0' || isspace((unsigned char)isi[3]) || isi[3] == '(' ||
         isi[3] == ')')) {
      hasil->tipe = TIPE_TOLEK_KATA_KUNCI_EKSTERNAL;
      hasil->teks = awal;
      hasil->panjang = 3;
      return isi + 3;
    }

    if (strncmp(isi, "pub", 3) == 0 &&
        (isi[3] == '\0' || isspace((unsigned char)isi[3]) || isi[3] == '(' ||
         isi[3] == ')')) {
      hasil->tipe = TIPE_TOLEK_KATA_KUNCI_PUBLIK;
      hasil->teks = awal;
      hasil->panjang = 3;
      return isi + 3;
    }

    if (strncmp(isi, "unt", 3) == 0 &&
        (isi[3] == '\0' || isspace((unsigned char)isi[3]) || isi[3] == '(' ||
         isi[3] == ')')) {
      hasil->tipe = TIPE_TOLEK_TIPE_DATA_UNTAIAN;
      hasil->teks = awal;
      hasil->panjang = 3;
      return isi + 3;
    }

    if (strncmp(isi, "muat", 4) == 0 && batas_kata_kunci(isi, 4)) {
      hasil->tipe = TIPE_TOLEK_TIPE_DATA_MUAT;
      hasil->teks = awal;
      hasil->panjang = 4;
      return isi + 4;
    }

    if (strncmp(isi, "b32", 3) == 0 && batas_kata_kunci(isi, 3)) {
      hasil->tipe = TIPE_TOLEK_TIPE_DATA_BILANGAN;
      hasil->teks = awal;
      hasil->panjang = 3;
      return isi + 3;
    }

    if (strncmp(isi, "windows", 7) == 0 && batas_kata_kunci(isi, 7)) {
      hasil->tipe = TIPE_TOLEK_SO_WINDOWS;
      hasil->teks = awal;
      hasil->panjang = 7;
      return isi + 7;
    }

    if (strncmp(isi, "linux", 5) == 0 && batas_kata_kunci(isi, 5)) {
      hasil->tipe = TIPE_TOLEK_SO_LINUX;
      hasil->teks = awal;
      hasil->panjang = 5;
      return isi + 5;
    }

    if (strncmp(isi, "intel_32", 8) == 0 && batas_kata_kunci(isi, 8)) {
      hasil->tipe = TIPE_TOLEK_ARS_INTEL_32;
      hasil->teks = awal;
      hasil->panjang = 8;
      return isi + 8;
    }

    if (strncmp(isi, "intel_64", 8) == 0 && batas_kata_kunci(isi, 8)) {
      hasil->tipe = TIPE_TOLEK_ARS_INTEL_64;
      hasil->teks = awal;
      hasil->panjang = 8;
      return isi + 8;
    }

    while (*isi && (isalnum((unsigned char)*isi) || *isi == '_'))
      isi++;
    hasil->tipe = TIPE_TOLEK_PENGENAL;
    hasil->teks = awal;
    hasil->panjang = (int)(isi - awal);
    return isi;
  }

  if (*isi == '_') {
    const char *awal = isi;
    while (*isi && (isalnum((unsigned char)*isi) || *isi == '_'))
      isi++;
    hasil->tipe = TIPE_TOLEK_PENGENAL;
    hasil->teks = awal;
    hasil->panjang = (int)(isi - awal);
    return isi;
  }

  if (*isi == '(') {
    hasil->tipe = TIPE_TOLEK_KURUNG_BULAT_BUKA;
    hasil->teks = isi;
    hasil->panjang = 1;
    return isi + 1;
  }

  if (*isi == ')') {
    hasil->tipe = TIPE_TOLEK_KURUNG_BULAT_TUTUP;
    hasil->teks = isi;
    hasil->panjang = 1;
    return isi + 1;
  }

  if (*isi == '{') {
    hasil->tipe = TIPE_TOLEK_KURUNG_KURAWAT_BUKA;
    hasil->teks = isi;
    hasil->panjang = 1;
    return isi + 1;
  }

  if (*isi == '}') {
    hasil->tipe = TIPE_TOLEK_KURUNG_KURAWAT_TUTUP;
    hasil->teks = isi;
    hasil->panjang = 1;
    return isi + 1;
  }

  if (*isi == ',') {
    hasil->tipe = TIPE_TOLEK_KOMA;
    hasil->teks = isi;
    hasil->panjang = 1;
    return isi + 1;
  }

  if (*isi == '=') {
    hasil->tipe = TIPE_TOLEK_OPERASI_ISI;
    hasil->teks = isi;
    hasil->panjang = 1;
    return isi + 1;
  }

  if (*isi == '.') {
    hasil->tipe = TIPE_TOLEK_TITIK;
    hasil->teks = isi;
    hasil->panjang = 1;
    return isi + 1;
  }

  hasil->tipe = TIPE_TOLEK_TIDAK_DIKETAHUI;
  hasil->teks = isi;
  hasil->panjang = 1;
  return isi + 1;
}