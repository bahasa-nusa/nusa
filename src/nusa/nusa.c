// Copyright 2026 Pengembang Bahasa Pemrograman Nusa
// SPDX-License-Identifier: Apache-2.0

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nusa/berkas.h"
#include "nusa/optimasi.h"
#include "nusa/pengurai.h"
#include "nusa/peleksim.h"
#include "nusa/pesemantik.h"
#include "nusa/target.h"

#include "nusa/urai_arg.h"
#include "nusa/bagkang_llvm.h"
#include "nusa/jembat.h"

void cetak_info() {
  printf("Penggunaan: nusa <perintah> [berkas]\n\n");

  printf("Perintah:\n");
  printf("  versi                                                    Untuk melihat versi.\n");
  printf("  info                                                     Untuk melihat informasi penggunaan.\n");
  printf("  leks <berkas>                                            Analisis leksim.\n");
  printf("  urai <berkas>                                            Penguraian pohon sintaksis abstrak (PSA).\n");
  printf("  smtk <berkas>                                            Pemeriksaan semantik.\n");
  printf("  ra [opt] <target> <berkas>                               Representasi antara.\n");
  printf("  llvm [opt] <ra|rkt> <target> <berkas> [<berkas-keluar>]  Backend LLVM.\n");
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

static void kumpul_leksim(const char *isi, const char *jalur, bool titik_masuk) {
  InfoBer *ib = malloc(sizeof(InfoBer));
  ib->isi = salin(isi);
  ib->jalur = salin(jalur);
  ib->titik_masuk = titik_masuk;
  ib->lanjut = NULL;

  const char *ptr = isi;
  Leksim leksim;
  bool di_atas = true;
  while ((ptr = peleksim(ptr, &leksim)) && leksim.tipe != TIPE_LEKSIM_AKHIR) {
    if (leksim.tipe == TIPE_LEKSIM_KOMENTAR)
      continue;

    if (leksim.tipe == TIPE_LEKSIM_TIPE_DATA_MUAT && di_atas) {
      Leksim lanjut;
      const char *ptr_lanjut = ptr;
      while ((ptr_lanjut = peleksim(ptr_lanjut, &lanjut)) &&
             lanjut.tipe != TIPE_LEKSIM_AKHIR) {
        if (lanjut.tipe == TIPE_LEKSIM_KOMENTAR)
          continue;
        if (lanjut.tipe == TIPE_LEKSIM_PENGENAL)
          continue;
        if (lanjut.tipe == TIPE_LEKSIM_NILAI_UNTAIAN) {
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
                    kumpul_leksim(sub, res, false);
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

static void cetak_leksim_terkumpul(void) {
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
    Leksim leksim;
    while ((ptr = peleksim(ptr, &leksim)) && leksim.tipe != TIPE_LEKSIM_AKHIR) {
      if (leksim.panjang > max_len)
        max_len = leksim.panjang;
    }
    if (max_len < 22)
      max_len = 22;

    ptr = cur->isi;
    while ((ptr = peleksim(ptr, &leksim)) && leksim.tipe != TIPE_LEKSIM_AKHIR) {
      printf("%*.*s | %s\n", max_len, leksim.panjang, leksim.teks,
             nama_leksim(leksim.tipe));
    }
    printf("%*s | %s\n", max_len, "", nama_leksim(TIPE_LEKSIM_AKHIR));

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
    if (!arg.leks && !arg.urai && !arg.smtk && !arg.ra && !arg.opt && !arg.llvm) {
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

    // Leksim
    if (arg.leks) {
      printf("Leksim:\n");
      bersihkan_daftar_dimuat();
      tandai_berkas_dimuat(arg.berkas_masuk);
      kumpul_leksim(isi_berkas, arg.berkas_masuk, true);
      cetak_leksim_terkumpul();
    }

    // PSA (Urai)
    if (arg.urai || arg.smtk || arg.ra || arg.opt || arg.llvm) {
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
        if (arg.ra || arg.opt) {
          if (!arg.target) {
            printf("Perintah ra memerlukan <target>\n");
            return 1;
          }
          const Target *t = set_target(arg.target);
          if (!t) {
            return 1;
          }
          InstruksiRA *ra = bangkitkan_ra(psa);
          InstruksiRA *ra_imp = tambah_impor_ra(ra, ra);
          if (arg.ra) {
            if (arg.opt) {
              InstruksiRA *ra_opt = optimalkan(ra);
              InstruksiRA *ra_opt_imp = tambah_impor_ra(ra_opt, ra_opt);
              printf("\nOptimasi:\n");
              cetak_ra_permodul(ra_opt_imp);
              bersihkan_ra(ra_imp);
              bersihkan_ra(ra_opt_imp);
            } else {
              printf("\nRA:\n");
              cetak_ra_permodul(ra_imp);
              bersihkan_ra(ra_imp);
            }
          }
        }

        // llvm backend
        if (arg.llvm) {
          if (!arg.target) {
            printf("Perintah llvm memerlukan <target>\n");
            return 1;
          }
          if (!arg.bentuk || (strcmp(arg.bentuk, "ra") != 0 && strcmp(arg.bentuk, "rkt") != 0)) {
            printf("Perintah llvm memerlukan <ra|rkt>\n");
            return 1;
          }
          if (!bagkang_llvm_target_tersedia(arg.target)) {
            printf("Target tidak didukung LLVM: %s\n", arg.target);
            return 1;
          }
          
          bagkang_llvm_daftarkan();

          InstruksiRA *ra = bangkitkan_ra(psa);
          InstruksiRA *ra_imp = tambah_impor_ra(ra, ra);
          InstruksiRA *ra_opt = optimalkan(ra);
          InstruksiRA *ra_opt_imp = tambah_impor_ra(ra_opt, ra_opt);

          OpsiBackend opsi = {
            .optimasi = arg.opt,
            .so = NULL,
            .target = arg.target,
            .bentuk = arg.bentuk,
            .berkas_keluar = arg.berkas_keluar
          };

          int hasil = jembat_jalankan("llvm", ra_opt_imp, &opsi);
          bersihkan_ra(ra_imp);
          bersihkan_ra(ra_opt_imp);

          if (hasil != 0) {
            return 1;
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