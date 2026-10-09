// Copyright 2026 Pengembang Bahasa Pemrograman Nusa
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <string.h>
#include "nusa/jembat.h"

static const char *tercatat_nama = NULL;
static const InstruksiRA *tercatat_ra = NULL;
static OpsiBackend tercatat_opsi;
static int jumlah_panggilan = 0;

static int backend_palsu(const InstruksiRA *ra, const OpsiBackend *opsi) {
  jumlah_panggilan++;
  tercatat_ra = ra;
  if (opsi) {
    tercatat_opsi = *opsi;
  }
  return 0;
}

int main(void) {
  jembat_daftarkan("palsu", backend_palsu);

  InstruksiRA dummy_ra = {0};
  OpsiBackend opsi = {
    .optimasi = true,
    .so = "test.so",
    .target = "x86_64",
    .bentuk = "ra",
    .berkas_keluar = NULL
  };

  int hasil = jembat_jalankan("palsu", &dummy_ra, &opsi);
  if (hasil != 0 || jumlah_panggilan != 1 || tercatat_ra != &dummy_ra ||
      !tercatat_opsi.optimasi || strcmp(tercatat_opsi.so, "test.so") != 0 ||
      strcmp(tercatat_opsi.target, "x86_64") != 0 ||
      strcmp(tercatat_opsi.bentuk, "ra") != 0) {
    printf("Galat: jembat_jalankan gagal mencocokkan argumen backend palsu\n");
    return 1;
  }

  int hasil_tidak_ada = jembat_jalankan("tidak_ada", &dummy_ra, &opsi);
  if (hasil_tidak_ada != -1) {
    printf("Galat: jembat_jalankan pada backend tidak ada harusnya mengembalikan -1, dapat %d\n", hasil_tidak_ada);
    return 1;
  }

  printf("OK\n");
  return 0;
}
