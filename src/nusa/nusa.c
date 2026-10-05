// Copyright 2026 Pengembang Bahasa Pemrograman Nusa
// SPDX-License-Identifier: Apache-2.0

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nusa/berkas.h"
#include "nusa/brkt.h"
#include "nusa/optimasi.h"
#include "nusa/pengurai.h"
#include "nusa/penolek.h"
#include "nusa/pesemantik.h"
#include "nusa/target.h"

#include "nusa/urai_arg.h"

void cetak_info() {
  printf("Penggunaan: nusa <argumen> [berkas]\n\n");
  printf("Opsi:\n");
  printf("-v, --versi                                            Untuk melihat "
         "versi.\n");
  printf("-i, --info                                             Untuk melihat "
         "informasi penggunaan.\n");
  printf("-tolek                                                 Analisis "
         "token.\n");
  printf("-urai                                                  Penguraian "
         "pohon sintaksis abstrak (PSA).\n");
  printf("-smtk                                                  Pemeriksaan "
         "semantik.\n");
  printf("-ra <sistem-operasi> <arsitektur>                      Representasi "
         "Antara.\n");
  printf("-opt <sistem-operasi> <arsitektur>                     Optimasi.\n");
  printf("-brkt <sistem-operasi> <arsitektur> [-o <berkas>]      Bahasa "
         "Rakitan.\n\n");
  printf("Sistem Operasi Yang Tersedia:\n");
  printf("  windows\n");
  printf("  linux\n\n");
  printf("Arsitektur Yang Tersedia:\n");
  printf("  intel_64\n");
  printf("  intel_32\n");
}

static char *salin(const char *s) {
  if (!s)
    return NULL;

  size_t p = strlen(s) + 1;
  char *r = malloc(p);
  if (r)
    memcpy(r, s, p);

  return r;
}

typedef struct InfoBer {
  char *isi;
  char *jalur;
  bool titik_masuk;
  struct InfoBer *lanjut;
} InfoBer;

static InfoBer *daftar_info = NULL;

static void kumpul_tolek(const char *isi, const char *jalur, bool titik_masuk) {
  InfoBer *ib = malloc(sizeof(InfoBer));
  ib->isi = salin(isi);
  ib->jalur = salin(jalur);
  ib->titik_masuk = titik_masuk;
  ib->lanjut = NULL;

  const char *ptr = isi;
  Tolek tolek;
  bool di_atas = true;
  while ((ptr = penolek(ptr, &tolek)) && tolek.tipe != TIPE_TOLEK_AKHIR) {
    if (tolek.tipe == TIPE_TOLEK_KOMENTAR)
      continue;

    if (tolek.tipe == TIPE_TOLEK_NILAI_UNTAIAN && di_atas) {
      int len = tolek.panjang;

      if (len >= 2 && tolek.teks[0] == '\'' && tolek.teks[len - 1] == '\'') {
        int path_len = len - 2;

        if (path_len > 0) {
          char *nama = malloc(path_len + 1);

          if (nama) {
            strncpy(nama, tolek.teks + 1, path_len);
            nama[path_len] = '\0';

            char *res = gabung_jalur_relatif(jalur, nama);
            free(nama);

            if (res && !berkas_sudah_dimuat(res)) {
              tandai_berkas_dimuat(res);

              const char *sub = baca_berkas(res);
              if (sub) {
                kumpul_tolek(sub, res, false);
                bersihkan_berkas(sub);
              }
            }

            free(res);
          }
        }
      }

      continue;
    }

    di_atas = false;
  }

  if (!daftar_info) {
    daftar_info = ib;
  } else {
    InfoBer *cur = daftar_info;
    while (cur->lanjut)
      cur = cur->lanjut;
    cur->lanjut = ib;
  }
}

static void cetak_tolek_terkumpul(void) {
  int nomor = 0;
  InfoBer *cur = daftar_info;
  while (cur) {
    nomor++;
    if (nomor > 1)
      printf("\n");
    printf("Berkas %d: %s%s\n", nomor, cur->jalur,
           cur->titik_masuk ? " (titik masuk)" : "");

    int max_len = 0;
    const char *ptr = cur->isi;
    Tolek tolek;
    while ((ptr = penolek(ptr, &tolek)) && tolek.tipe != TIPE_TOLEK_AKHIR) {
      if (tolek.panjang > max_len)
        max_len = tolek.panjang;
    }
    if (max_len < 22)
      max_len = 22;

    ptr = cur->isi;
    while ((ptr = penolek(ptr, &tolek)) && tolek.tipe != TIPE_TOLEK_AKHIR) {
      printf("%*.*s | %s\n", max_len, tolek.panjang, tolek.teks,
             nama_tolek(tolek.tipe));
    }
    printf("%*s | %s\n", max_len, "", nama_tolek(TIPE_TOLEK_AKHIR));

    InfoBer *next = cur->lanjut;
    free(cur->isi);
    free(cur->jalur);
    free(cur);
    cur = next;
  }

  daftar_info = NULL;
}

int main(int argc, char **argv) {
  Arg arg = urai_arg(argc, argv);

  if (arg.versi) {
    printf("nusa v0.0.0\n");
    return 0;
  }

  if (arg.info) {
    cetak_info();
    return 0;
  }

  if (arg.input_file) {
    if (!arg.tolek && !arg.urai && !arg.smtk && !arg.ra && !arg.opt &&
        !arg.brkt) {
      printf("Argumen tidak valid.\n");
      cetak_info();
      return 1;
    }

    const char *isi_berkas = baca_berkas(arg.input_file);
    if (!isi_berkas) {
      printf("Gagal baca: %s\n", arg.input_file);
      return 1;
    }

    // Tolek
    if (arg.tolek) {
      printf("Tolek:\n");
      bersihkan_daftar_dimuat();
      tandai_berkas_dimuat(arg.input_file);
      kumpul_tolek(isi_berkas, arg.input_file, true);
      cetak_tolek_terkumpul();
    }

    // PSA (Urai)
    if (arg.urai || arg.smtk || arg.ra || arg.opt || arg.brkt) {
      if (arg.urai) {
        printf("PSA:\n");
      }
      bersihkan_daftar_dimuat();
      tandai_berkas_dimuat(arg.input_file);

      PSA *psa = urai(isi_berkas, arg.input_file);
      if (psa) {
        if (arg.urai) {
          cetak_psa(psa, 0);
        }
        if (arg.smtk) {
          printf("\nPesemantik:\n");
          int galat = pesemantik(psa);
          if (!galat)
            printf("Tidak ada kesalahan\n");
        }
        if (arg.ra || arg.opt || arg.brkt) {
          if (!arg.so || !arg.arsitektur) {
            printf("-ra, -opt, atau -brkt memerlukan <sistem-operasi> dan "
                   "<arsitektur>\n");
            return 1;
          }
          if (strcmp(arg.so, "windows") != 0 && strcmp(arg.so, "linux") != 0) {
            printf("<sistem-operasi> saat ini hanya mendukung 'windows' atau "
                   "'linux'\n");
            return 1;
          }
          if (strcmp(arg.arsitektur, "intel_64") != 0 &&
              strcmp(arg.arsitektur, "intel_32") != 0) {
            printf("<arsitektur> saat ini hanya mendukung 'intel_64' atau "
                   "'intel_32'\n");
            return 1;
          }
          set_target(arg.so, arg.arsitektur);
          InstruksiRA *ra = bangkitkan_ra(psa, arg.so, arg.arsitektur);
          InstruksiRA *ra_imp = tambah_impor_ra(ra, ra);
          if (arg.ra) {
            printf("\nRA:\n");
            cetak_ra_permodul(ra_imp);
          }
          if (arg.opt || arg.brkt) {
            InstruksiRA *ra_opt = optimalkan(ra);
            InstruksiRA *ra_opt_imp = tambah_impor_ra(ra_opt, ra_opt);
            if (arg.opt) {
              printf("\nOptimasi:\n");
              cetak_ra_permodul(ra_opt_imp);
            }
            if (arg.brkt) {
              bangkitkan_brkt(ra_opt_imp, arg.output);
            }
            bersihkan_ra(ra_imp);
            bersihkan_ra(ra_opt_imp);
          } else {
            bersihkan_ra(ra_imp);
          }
        }
        bersihkan_psa(psa);
      } else {
        printf("Gagal mengurai: %s\n", pesan_urai());
      }
    }

    bersihkan_daftar_dimuat();
    bersihkan_berkas(isi_berkas);
  } else {
    if (argc > 1)
      printf("Argumen tidak valid.\n");
    cetak_info();
  }

  return 0;
}