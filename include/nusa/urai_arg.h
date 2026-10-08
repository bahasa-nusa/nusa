// Copyright 2026 Pengembang Bahasa Pemrograman Nusa
// SPDX-License-Identifier: Apache-2.0

#ifndef NUSA_URAI_ARG_H
#define NUSA_URAI_ARG_H

#include <stdbool.h>

typedef struct {
  bool versi;
  bool info;
  const char *berkas_masuk;
  bool leks;
  bool urai;
  bool smtk;
  bool ra;
  bool opt;
  bool rkt;
  const char *so;
  const char *arsitektur;
  const char *berkas_keluar;
} Arg;

Arg urai_arg(int jum_arg, char **isi_arg);

#endif
