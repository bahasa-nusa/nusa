// Copyright 2026 Pengembang Bahasa Pemrograman Nusa
// SPDX-License-Identifier: Apache-2.0

#include "nusa/pengurai.h"
#include "nusa/berkas.h"
#include "nusa/penolek.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *pesan_kesalahan = NULL;
static char *akar_dasar = NULL;

const char *pesan_urai() { return pesan_kesalahan; }

static char *salin_n(const char *s, int n) {
  if (!s)
    return NULL;
  char *r = malloc((size_t)n + 1);
  if (!r)
    return NULL;
  memcpy(r, s, n);
  r[n] = '\0';
  return r;
}

static char *salin(const char *s) {
  if (!s)
    return NULL;
  return salin_n(s, (int)strlen(s));
}

static PSA *buat_node(TipePSA tipe, const char *teks, int panjang) {
  PSA *node = malloc(sizeof(PSA));
  node->tipe = tipe;
  node->teks = salin_n(teks, panjang);
  node->panjang = panjang;
  node->jalur = NULL;
  node->modul = NULL;
  node->titik_masuk = false;
  node->anak = NULL;
  node->jumlah_anak = 0;

  return node;
}

static char *modul_dari_jalur(const char *jalur, const char *akar_dasar) {
  const char *mulai = jalur;

  size_t pjg_dasar = strlen(akar_dasar);
  if (pjg_dasar > 0 && strncmp(jalur, akar_dasar, pjg_dasar) == 0) {
    mulai = jalur + pjg_dasar;
    while (*mulai == '/' || *mulai == '\\')
      mulai++;
  }

  char *r = salin(mulai);
  if (!r)
    return NULL;

  for (char *c = r; *c; c++) {
    if (!((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z') ||
          (*c >= '0' && *c <= '9'))) {
      *c = '_';
    }
  }

  return r;
}

static void tambah_anak(PSA *induk, PSA *anak) {
  induk->jumlah_anak++;
  induk->anak = realloc(induk->anak, sizeof(PSA *) * induk->jumlah_anak);
  induk->anak[induk->jumlah_anak - 1] = anak;
}

void bersihkan_psa(PSA *node) {
  if (!node)
    return;

  for (int i = 0; i < node->jumlah_anak; i++) {
    bersihkan_psa(node->anak[i]);
  }

  free(node->anak);
  free(node->teks);
  free(node->jalur);
  free(node->modul);
  free(node);
}

void cetak_psa(const PSA *node, int indent) {
  if (!node)
    return;

  if (node->tipe == PSA_PROGRAM) {
    bool pertama = true;
    for (int i = 0; i < node->jumlah_anak; i++) {
      const PSA *anak = node->anak[i];
      if (anak->tipe != PSA_BERKAS)
        continue;
      if (!pertama)
        printf("\n");
      pertama = false;
      printf("Berkas %d: %s%s\n", i + 1, anak->jalur,
             anak->titik_masuk ? " (titik masuk)" : "");
      for (int j = 0; j < anak->jumlah_anak; j++) {
        cetak_psa(anak->anak[j], 1);
      }
    }
    return;
  }

  for (int i = 0; i < indent; i++)
    printf("  ");
  static const char *nama_tipe[] = {"PROGRAM",
                                    "BERKAS",
                                    "PANGGILAN",
                                    "DEKLARASI",
                                    "PENGENAL",
                                    "NILAI UNTAIAN",
                                    "NILAI BILANGAN",
                                    "KATA KUNCI",
                                    "KATA KUNCI PUBLIK",
                                    "TIPE DATA B32",
                                    "TIPE DATA UNTAIAN",
                                    "BLOK OS ARSITEKTUR",
                                    "SO WINDOWS",
                                    "SO LINUX",
                                    "ARSITEKTUR INTEL 32",
                                    "ARSITEKTUR INTEL 64"};
  printf("[%s] %.*s\n", nama_tipe[node->tipe], node->panjang, node->teks);

  for (int i = 0; i < node->jumlah_anak; i++) {
    cetak_psa(node->anak[i], indent + 1);
  }
}

static bool deklarasi_menunggu(const char *ptr) {
  Tolek t;
  while ((ptr = penolek(ptr, &t)) && t.tipe != TIPE_TOLEK_AKHIR) {
    if (t.tipe == TIPE_TOLEK_KURUNG_BULAT_TUTUP)
      break;
    if (t.tipe == TIPE_TOLEK_TIPE_DATA_UNTAIAN ||
        t.tipe == TIPE_TOLEK_TIPE_DATA_BILANGAN) {
      return true;
    }
  }
  return false;
}

static PSA *urai_berkas(const char *isi, const char *jalur, bool titik_masuk) {
  PSA *akar = buat_node(PSA_PROGRAM, "program", 7);
  PSA *modul = buat_node(PSA_BERKAS, NULL, 0);
  modul->jalur = salin(jalur);
  modul->titik_masuk = titik_masuk;
  modul->modul = modul_dari_jalur(jalur, akar_dasar);

  PSA *last_decl = NULL;

  Tolek tolek;
  const char *ptr = isi;
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

              const char *sub_isi = baca_berkas(res);
              if (sub_isi) {
                PSA *sub = urai_berkas(sub_isi, res, false);

                for (int i = 0; i < sub->jumlah_anak - 1; i++) {
                  tambah_anak(akar, sub->anak[i]);
                  sub->anak[i] = NULL;
                }

                PSA *self = sub->anak[sub->jumlah_anak - 1];

                sub->anak[sub->jumlah_anak - 1] = NULL;
                free(sub->anak);
                free(sub);
                tambah_anak(akar, self);
                bersihkan_berkas(sub_isi);
              }
            }

            free(res);
          }
        }
      }

      continue;
    }

    di_atas = false;

    if (tolek.tipe == TIPE_TOLEK_PENGENAL) {
      Tolek lanjut;
      const char *ptr_lanjut = penolek(ptr, &lanjut);

      if (lanjut.tipe == TIPE_TOLEK_KURUNG_BULAT_BUKA) {
        ptr = ptr_lanjut;

        Tolek lanjut2;
        const char *ptr_lanjut2 = penolek(ptr, &lanjut2);

        bool adalah_deklarasi = false;
        if (lanjut2.tipe == TIPE_TOLEK_PENGENAL) {
          adalah_deklarasi = deklarasi_menunggu(ptr_lanjut2);
        }

        if (adalah_deklarasi) {
          PSA *node = buat_node(PSA_DEKLARASI, tolek.teks, tolek.panjang);
          node->modul = salin(modul->modul);
          ptr = ptr_lanjut;

          while ((ptr = penolek(ptr, &lanjut)) &&
                 lanjut.tipe != TIPE_TOLEK_KURUNG_BULAT_TUTUP &&
                 lanjut.tipe != TIPE_TOLEK_AKHIR) {
            if (lanjut.tipe == TIPE_TOLEK_PENGENAL) {
              tambah_anak(node,
                          buat_node(PSA_PENGENAL, lanjut.teks, lanjut.panjang));
            } else if (lanjut.tipe == TIPE_TOLEK_TIPE_DATA_UNTAIAN) {
              tambah_anak(node, buat_node(PSA_TIPE_DATA_UNTAIAN, lanjut.teks,
                                          lanjut.panjang));
            } else if (lanjut.tipe == TIPE_TOLEK_TIPE_DATA_BILANGAN) {
              tambah_anak(node, buat_node(PSA_TIPE_DATA_BILANGAN, lanjut.teks,
                                          lanjut.panjang));
            } else if (lanjut.tipe == TIPE_TOLEK_KATA_KUNCI_EKSTERNAL) {
              tambah_anak(
                  node, buat_node(PSA_KATA_KUNCI, lanjut.teks, lanjut.panjang));
            }
          }

          if (lanjut.tipe == TIPE_TOLEK_KURUNG_BULAT_TUTUP) {
            ptr = penolek(ptr, &lanjut);

            if (lanjut.tipe == TIPE_TOLEK_KATA_KUNCI_EKSTERNAL) {
              tambah_anak(
                  node, buat_node(PSA_KATA_KUNCI, lanjut.teks, lanjut.panjang));
            } else if (lanjut.tipe == TIPE_TOLEK_KATA_KUNCI_PUBLIK) {
              tambah_anak(node, buat_node(PSA_KATA_KUNCI_PUBLIK, lanjut.teks,
                                          lanjut.panjang));
            }
          }

          tambah_anak(modul, node);

          Tolek next;
          const char *ptr_next = penolek(ptr, &next);
          if (next.tipe == TIPE_TOLEK_KURUNG_KURAWAT_BUKA) {
            ptr = ptr_next;

            const char *start = ptr;
            int depth = 1;
            const char *inner = ptr;
            Tolek t;
            while (depth > 0 && (inner = penolek(inner, &t)) &&
                   t.tipe != TIPE_TOLEK_AKHIR) {
              if (t.tipe == TIPE_TOLEK_KURUNG_KURAWAT_BUKA)
                depth++;
              else if (t.tipe == TIPE_TOLEK_KURUNG_KURAWAT_TUTUP)
                depth--;
            }
            size_t len = (size_t)(inner - start);
            char *sub = malloc(len + 1);
            memcpy(sub, start, len);
            sub[len] = '\0';
            PSA *sub_modul = urai_berkas(sub, modul->jalur, false);
            for (int i = 0; i < sub_modul->jumlah_anak; i++) {
              PSA *berkas_anak = sub_modul->anak[i];
              if (berkas_anak->tipe == PSA_BERKAS) {
                for (int j = 0; j < berkas_anak->jumlah_anak; j++) {
                  tambah_anak(node, berkas_anak->anak[j]);
                  berkas_anak->anak[j] = NULL;
                }
              }
            }
            bersihkan_psa(sub_modul);
            free(sub);
            ptr = inner;
          }
          last_decl = node;
        } else {
          PSA *node = buat_node(PSA_PANGGILAN, tolek.teks, tolek.panjang);
          node->modul = salin(modul->modul);
          ptr = ptr_lanjut;

          while ((ptr = penolek(ptr, &lanjut)) &&
                 lanjut.tipe != TIPE_TOLEK_KURUNG_BULAT_TUTUP &&
                 lanjut.tipe != TIPE_TOLEK_AKHIR) {
            if (lanjut.tipe == TIPE_TOLEK_PENGENAL) {
              tambah_anak(node,
                          buat_node(PSA_PENGENAL, lanjut.teks, lanjut.panjang));
            } else if (lanjut.tipe == TIPE_TOLEK_NILAI_UNTAIAN) {
              tambah_anak(node, buat_node(PSA_NILAI_UNTAIAN, lanjut.teks,
                                          lanjut.panjang));
            } else if (lanjut.tipe == TIPE_TOLEK_NILAI_BILANGAN) {
              tambah_anak(node, buat_node(PSA_NILAI_BILANGAN, lanjut.teks,
                                          lanjut.panjang));
            } else if (lanjut.tipe == TIPE_TOLEK_TIPE_DATA_UNTAIAN) {
              tambah_anak(node, buat_node(PSA_TIPE_DATA_UNTAIAN, lanjut.teks,
                                          lanjut.panjang));
            } else if (lanjut.tipe == TIPE_TOLEK_TIPE_DATA_BILANGAN) {
              tambah_anak(node, buat_node(PSA_TIPE_DATA_BILANGAN, lanjut.teks,
                                          lanjut.panjang));
            } else if (lanjut.tipe == TIPE_TOLEK_KATA_KUNCI_EKSTERNAL) {
              tambah_anak(
                  node, buat_node(PSA_KATA_KUNCI, lanjut.teks, lanjut.panjang));
            } else if (lanjut.tipe == TIPE_TOLEK_KATA_KUNCI_PUBLIK) {
              tambah_anak(node, buat_node(PSA_KATA_KUNCI_PUBLIK, lanjut.teks,
                                          lanjut.panjang));
            }
          }

          tambah_anak(modul, node);
          last_decl = NULL;
        }
      } else {
        PSA *node = buat_node(PSA_PENGENAL, tolek.teks, tolek.panjang);
        tambah_anak(modul, node);
      }
    } else if (tolek.tipe == TIPE_TOLEK_KATA_KUNCI_EKSTERNAL) {
      tambah_anak(modul, buat_node(PSA_KATA_KUNCI, tolek.teks, tolek.panjang));
    } else if (tolek.tipe == TIPE_TOLEK_KATA_KUNCI_PUBLIK) {
      tambah_anak(modul,
                  buat_node(PSA_KATA_KUNCI_PUBLIK, tolek.teks, tolek.panjang));
    } else if (tolek.tipe == TIPE_TOLEK_TIPE_DATA_UNTAIAN) {
      tambah_anak(modul,
                  buat_node(PSA_TIPE_DATA_UNTAIAN, tolek.teks, tolek.panjang));
    } else if (tolek.tipe == TIPE_TOLEK_TIPE_DATA_BILANGAN) {
      tambah_anak(modul,
                  buat_node(PSA_TIPE_DATA_BILANGAN, tolek.teks, tolek.panjang));
    } else if (tolek.tipe == TIPE_TOLEK_SO_WINDOWS ||
               tolek.tipe == TIPE_TOLEK_SO_LINUX ||
               tolek.tipe == TIPE_TOLEK_ARS_INTEL_32 ||
               tolek.tipe == TIPE_TOLEK_ARS_INTEL_64) {
      PSA *blok = buat_node(PSA_BLOK_OS_ARSITEKTUR, NULL, 0);
      blok->modul = salin(modul->modul);
      PSA *parent = last_decl ? last_decl : modul;
      tambah_anak(parent, blok);
      last_decl = NULL;

      TipePSA tipe_target = PSA_BLOK_OS_ARSITEKTUR;
      switch (tolek.tipe) {
      case TIPE_TOLEK_SO_WINDOWS:
        tipe_target = PSA_SO_WINDOWS;
        break;
      case TIPE_TOLEK_SO_LINUX:
        tipe_target = PSA_SO_LINUX;
        break;
      case TIPE_TOLEK_ARS_INTEL_32:
        tipe_target = PSA_ARS_INTEL_32;
        break;
      default:
        tipe_target = PSA_ARS_INTEL_64;
        break;
      }
      tambah_anak(blok, buat_node(tipe_target, tolek.teks, tolek.panjang));

      while ((ptr = penolek(ptr, &tolek))) {
        if (tolek.tipe == TIPE_TOLEK_KOMA) {
          ptr = penolek(ptr, &tolek);
          if (tolek.tipe == TIPE_TOLEK_SO_WINDOWS)
            tipe_target = PSA_SO_WINDOWS;
          else if (tolek.tipe == TIPE_TOLEK_SO_LINUX)
            tipe_target = PSA_SO_LINUX;
          else if (tolek.tipe == TIPE_TOLEK_ARS_INTEL_32)
            tipe_target = PSA_ARS_INTEL_32;
          else if (tolek.tipe == TIPE_TOLEK_ARS_INTEL_64)
            tipe_target = PSA_ARS_INTEL_64;
          else
            break;
          tambah_anak(blok, buat_node(tipe_target, tolek.teks, tolek.panjang));
          continue;
        }
        break;
      }

      if (tolek.tipe == TIPE_TOLEK_KURUNG_KURAWAT_BUKA) {
        const char *start = ptr;
        int depth = 1;
        const char *inner = ptr;
        Tolek t;
        while (depth > 0 && (inner = penolek(inner, &t)) &&
               t.tipe != TIPE_TOLEK_AKHIR) {
          if (t.tipe == TIPE_TOLEK_KURUNG_KURAWAT_BUKA)
            depth++;
          else if (t.tipe == TIPE_TOLEK_KURUNG_KURAWAT_TUTUP)
            depth--;
        }

        size_t len = (size_t)(inner - start);
        char *sub = malloc(len + 1);
        memcpy(sub, start, len);
        sub[len] = '\0';

        PSA *sub_modul = urai_berkas(sub, modul->jalur, false);

        for (int i = 0; i < sub_modul->jumlah_anak; i++) {
          PSA *berkas_anak = sub_modul->anak[i];
          if (berkas_anak->tipe == PSA_BERKAS) {
            for (int j = 0; j < berkas_anak->jumlah_anak; j++) {
              tambah_anak(blok, berkas_anak->anak[j]);
              berkas_anak->anak[j] = NULL;
            }
          }
        }
        bersihkan_psa(sub_modul);
        free(sub);
        ptr = inner;
      }
    }
  }

  tambah_anak(akar, modul);
  return akar;
}

PSA *urai(const char *isi, const char *jalur_berkas) {
  char *kanon = jalur_kanonis(jalur_berkas);

  size_t pjg = strlen(kanon);
  while (pjg > 0 && kanon[pjg - 1] != '/' && kanon[pjg - 1] != '\\')
    pjg--;

  free(akar_dasar);
  akar_dasar = salin_n(kanon, (int)pjg);

  PSA *akar = urai_berkas(isi, kanon, true);

  free(kanon);
  return akar;
}