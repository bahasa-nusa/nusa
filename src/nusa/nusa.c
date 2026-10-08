// Copyright 2026 Pengembang Bahasa Pemrograman Nusa
// SPDX-License-Identifier: Apache-2.0

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nusa/berkas.h"
#include "nusa/rkt.h"
#include "nusa/optimasi.h"
#include "nusa/pengurai.h"
#include "nusa/penolek.h"
#include "nusa/pesemantik.h"
#include "nusa/target.h"

#include "nusa/urai_arg.h"

void cetak_info() {
  printf("Penggunaan: nusa <perintah> [berkas]\n\n");

  printf("Perintah:\n");
  printf("  versi                                                         Untuk melihat versi.\n");
  printf("  info                                                          Untuk melihat informasi penggunaan.\n");
  printf("  nolek <berkas>                                                Analisis tolek.\n");
  printf("  urai <berkas>                                                 Penguraian pohon sintaksis abstrak (PSA).\n");
  printf("  smtk <berkas>                                                 Pemeriksaan semantik.\n");
  printf("  ra <sistem-operasi> <arsitektur> <berkas>                     Representasi antara.\n");
  printf("  opt <sistem-operasi> <arsitektur> <berkas>                    Optimasi.\n");
  printf("  rkt <sistem-operasi> <arsitektur> <berkas> [<berkas-keluar>]  Bahasa rakitan.\n\n");

  printf("Sistem Operasi:\n");
  printf("  wins\n");
  printf("  linux\n\n");

  printf("Arsitektur:\n");
  printf("  intel64\n");
  printf("  intel32\n");
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

    if (tolek.tipe == TIPE_TOLEK_TIPE_DATA_MUAT && di_atas) {
      Tolek lanjut;
      const char *ptr_lanjut = ptr;
      while ((ptr_lanjut = penolek(ptr_lanjut, &lanjut)) &&
             lanjut.tipe != TIPE_TOLEK_AKHIR) {
        if (lanjut.tipe == TIPE_TOLEK_KOMENTAR)
          continue;
        if (lanjut.tipe == TIPE_TOLEK_PENGENAL)
          continue;
        if (lanjut.tipe == TIPE_TOLEK_NILAI_UNTAIAN) {
          int len = lanjut.panjang;
          char quote = lanjut.teks[0];
          if (len >= 2 && (quote == '\'' || quote == '"') &&
              lanjut.teks[len - 1] == quote) {
            int path_len = len - 2;
            if (path_len > 0) {
              char *nama = malloc(path_len + 1);
              if (nama) {
                strncpy(nama, lanjut.teks + 1, path_len);
                nama[path_len] = '\0';
                char *res = NULL;
                cari_berkas(nama, &res, NULL);
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
        break;
      }
      ptr = ptr_lanjut;
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

  if (arg.berkas_masuk) {
    if (!arg.nolek && !arg.urai && !arg.smtk && !arg.ra && !arg.opt &&
        !arg.rkt) {
      printf("Argumen tidak valid.\n");
      cetak_info();
      return 1;
    }

    direktori_berkas_utama = direktori_dari(arg.berkas_masuk);

    char *biner = jalur_biner();
    direktori_instalasi = direktori_dari(biner ? biner : argv[0]);
    free(biner);

    const char *isi_berkas = baca_berkas(arg.berkas_masuk);
    if (!isi_berkas) {
      printf("Gagal baca: %s\n", arg.berkas_masuk);
      return 1;
    }

    // Nolek
    if (arg.nolek) {
      printf("Nolek:\n");
      bersihkan_daftar_dimuat();
      tandai_berkas_dimuat(arg.berkas_masuk);
      kumpul_tolek(isi_berkas, arg.berkas_masuk, true);
      cetak_tolek_terkumpul();
    }

    // PSA (Urai)
    if (arg.urai || arg.smtk || arg.ra || arg.opt || arg.rkt) {
      if (arg.urai) {
        printf("PSA:\n");
      }
      bersihkan_daftar_dimuat();
      tandai_berkas_dimuat(arg.berkas_masuk);

      PSA *psa = urai(isi_berkas, arg.berkas_masuk);
      if (psa) {
        if (arg.urai) {
          cetak_psa(psa, 0);
        }
        if (arg.smtk) {
          printf("\nPesemantik:\n");
          int galat = pesemantik(psa);
          if (!galat)
            printf("Tidak ada kesalahan\n");
          else
            return 1;
        }
        if (arg.ra || arg.opt || arg.rkt) {
          if (!arg.so || !arg.arsitektur) {
            printf("Perintah ra, opt, atau rkt memerlukan <sistem-operasi> dan "
                   "<arsitektur>\n");
            return 1;
          }
          if (strcmp(arg.so, "wins") != 0 && strcmp(arg.so, "linux") != 0) {
            printf("<sistem-operasi> saat ini hanya mendukung 'wins' atau 'linux'\n");
            return 1;
          }
          if (strcmp(arg.arsitektur, "intel64") != 0 &&
              strcmp(arg.arsitektur, "intel32") != 0) {
            printf("<arsitektur> saat ini hanya mendukung 'intel64' atau 'intel32'\n");
            return 1;
          }
          set_target(arg.so, arg.arsitektur);
          InstruksiRA *ra = bangkitkan_ra(psa);
          InstruksiRA *ra_imp = tambah_impor_ra(ra, ra);
          if (arg.ra) {
            printf("\nRA:\n");
            cetak_ra_permodul(ra_imp);
          }
          if (arg.opt || arg.rkt) {
            InstruksiRA *ra_opt = optimalkan(ra);
            InstruksiRA *ra_opt_imp = tambah_impor_ra(ra_opt, ra_opt);
            if (arg.opt) {
              printf("\nOptimasi:\n");
              cetak_ra_permodul(ra_opt_imp);
            }
            if (arg.rkt) {
              bangkitkan_rkt(ra_opt_imp, arg.berkas_keluar);
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