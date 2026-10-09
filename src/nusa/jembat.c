// Copyright 2026 Pengembang Bahasa Pemrograman Nusa
// SPDX-License-Identifier: Apache-2.0

#include "nusa/jembat.h"
#include <string.h>

#define MAKS_BACKEND 8

typedef struct {
  char nama[64];
  FungsiBackend fungsi;
} EntriBackend;

static EntriBackend daftar_backend[MAKS_BACKEND];
static int jumlah_backend = 0;

void jembat_daftarkan(const char *nama, FungsiBackend f) {
  if (jumlah_backend < MAKS_BACKEND && nama && f) {
    strncpy(daftar_backend[jumlah_backend].nama, nama, 63);
    daftar_backend[jumlah_backend].nama[63] = '\0';
    daftar_backend[jumlah_backend].fungsi = f;
    jumlah_backend++;
  }
}

int jembat_jalankan(const char *nama, const InstruksiRA *ra, const OpsiBackend *opsi) {
  for (int i = 0; i < jumlah_backend; i++) {
    if (strcmp(daftar_backend[i].nama, nama) == 0) {
      return daftar_backend[i].fungsi(ra, opsi);
    }
  }
  return -1;
}
