#ifndef NUSA_URAI_ARG_H
#define NUSA_URAI_ARG_H

#include <stdbool.h>

typedef struct {
  bool versi;
  bool info;
  bool leks;
  bool urai;
  bool smtk;
  bool ra;
  bool opt;
  bool llvm;
  bool llvm_ar;
  const char *target;
  const char *bentuk;
  const char *mode;
  const char *berkas_keluar;
  const char **daftar_berkas_masuk;
  int jumlah_berkas_masuk;
} Arg;

Arg urai_arg(int jum_arg, char **isi_arg);

#endif
