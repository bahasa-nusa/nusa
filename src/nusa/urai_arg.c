#include "nusa/urai_arg.h"
#include <string.h>

Arg urai_arg(int jum_arg, char **isi_arg) {
  Arg argumen = {0};
  for (int i = 1; i < jum_arg; i++) {
    if (strcmp(isi_arg[i], "versi") == 0) {
      argumen.versi = true;
      break;
    } else if (strcmp(isi_arg[i], "info") == 0) {
      argumen.info = true;
      break;
    } else if (strcmp(isi_arg[i], "leks") == 0) {
      argumen.leks = true;
      if (i + 1 < jum_arg) {
        argumen.daftar_berkas_masuk = (const char **)&isi_arg[i + 1];
        argumen.jumlah_berkas_masuk = jum_arg - (i + 1);
      }
      break;
    } else if (strcmp(isi_arg[i], "urai") == 0) {
      argumen.urai = true;
      if (i + 1 < jum_arg) {
        argumen.daftar_berkas_masuk = (const char **)&isi_arg[i + 1];
        argumen.jumlah_berkas_masuk = jum_arg - (i + 1);
      }
      break;
    } else if (strcmp(isi_arg[i], "smtk") == 0) {
      argumen.smtk = true;
      if (i + 1 < jum_arg) {
        argumen.daftar_berkas_masuk = (const char **)&isi_arg[i + 1];
        argumen.jumlah_berkas_masuk = jum_arg - (i + 1);
      }
      break;
    } else if (strcmp(isi_arg[i], "ra") == 0) {
      argumen.ra = true;
      if (i + 1 < jum_arg && strcmp(isi_arg[i + 1], "opt") == 0) {
        argumen.opt = true;
        i++;
      }
      if (i + 1 < jum_arg) {
        argumen.target = isi_arg[++i];
      }
      if (i + 1 < jum_arg) {
        argumen.daftar_berkas_masuk = (const char **)&isi_arg[i + 1];
        argumen.jumlah_berkas_masuk = jum_arg - (i + 1);
      }
      break;
    } else if (strcmp(isi_arg[i], "llvm") == 0) {
      if (i + 1 < jum_arg && strcmp(isi_arg[i + 1], "ar") == 0) {
        argumen.llvm_ar = true;
        i++;
        if (i + 1 < jum_arg) {
          argumen.berkas_keluar = isi_arg[++i];
        }
        if (i + 1 < jum_arg) {
          argumen.daftar_berkas_masuk = (const char **)&isi_arg[i + 1];
          argumen.jumlah_berkas_masuk = jum_arg - (i + 1);
        }
        break;
      }
      argumen.llvm = true;
      if (i + 1 < jum_arg && strcmp(isi_arg[i + 1], "opt") == 0) {
        argumen.opt = true;
        i++;
      }
      if (i + 1 < jum_arg && (strcmp(isi_arg[i + 1], "ra") == 0 || strcmp(isi_arg[i + 1], "rkt") == 0)) {
        argumen.bentuk = isi_arg[++i];
      }
      if (i + 1 < jum_arg && (strcmp(isi_arg[i + 1], "teks") == 0 || strcmp(isi_arg[i + 1], "obj") == 0)) {
        argumen.mode = isi_arg[++i];
      }
      if (i + 1 < jum_arg) {
        argumen.target = isi_arg[++i];
      }
      if (i + 1 < jum_arg) {
        argumen.berkas_keluar = isi_arg[++i];
      }
      if (i + 1 < jum_arg) {
        argumen.daftar_berkas_masuk = (const char **)&isi_arg[i + 1];
        argumen.jumlah_berkas_masuk = jum_arg - (i + 1);
      }
      break;
    } else if (isi_arg[i][0] != '-') {
      argumen.daftar_berkas_masuk = (const char **)&isi_arg[i];
      argumen.jumlah_berkas_masuk = jum_arg - i;
      break;
    }
  }
  return argumen;
}
