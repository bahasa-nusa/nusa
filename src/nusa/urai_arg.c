// Copyright 2026 Pengembang Bahasa Pemrograman Nusa
// SPDX-License-Identifier: Apache-2.0

#include "nusa/urai_arg.h"
#include <string.h>

Arg urai_arg(int argc, char **argv) {
  Arg args = {0};
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "versi") == 0) {
      args.versi = true;
    } else if (strcmp(argv[i], "info") == 0) {
      args.info = true;
    } else if (strcmp(argv[i], "leks") == 0) {
      args.leks = true;
    } else if (strcmp(argv[i], "urai") == 0) {
      args.urai = true;
    } else if (strcmp(argv[i], "smtk") == 0) {
      args.smtk = true;
    } else if (strcmp(argv[i], "ra") == 0) {
      args.ra = true;
      if (i + 2 < argc) {
        args.so = argv[++i];
        args.arsitektur = argv[++i];
      }
    } else if (strcmp(argv[i], "opt") == 0) {
      args.opt = true;
      if (i + 2 < argc) {
        args.so = argv[++i];
        args.arsitektur = argv[++i];
      }
    } else if (argv[i][0] != '-') {
      args.berkas_masuk = argv[i];
    }
  }
  return args;
}
