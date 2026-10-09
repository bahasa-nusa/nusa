// Copyright 2026 Pengembang Bahasa Pemrograman Nusa
// SPDX-License-Identifier: Apache-2.0

#ifndef NUSA_JEMBAT_H
#define NUSA_JEMBAT_H

#include <stdbool.h>
#include "nusa/ra.h"

typedef struct {
  bool optimasi;
  const char *so;
  const char *target;
  const char *bentuk;
  const char *berkas_keluar;
} OpsiBackend;

typedef int (*FungsiBackend)(const InstruksiRA *ra, const OpsiBackend *opsi);

void jembat_daftarkan(const char *nama, FungsiBackend f);
int jembat_jalankan(const char *nama, const InstruksiRA *ra, const OpsiBackend *opsi);

#endif
