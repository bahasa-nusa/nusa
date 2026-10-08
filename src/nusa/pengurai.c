// Copyright 2026 Pengembang Bahasa Pemrograman Nusa
// SPDX-License-Identifier: Apache-2.0

#include "nusa/pengurai.h"
#include "nusa/berkas.h"
#include "nusa/peleksim.h"
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
                                    "MUAT",
                                    "TITIK"};
  printf("[%s] %.*s\n", nama_tipe[node->tipe], node->panjang, node->teks);

  for (int i = 0; i < node->jumlah_anak; i++) {
    cetak_psa(node->anak[i], indent + 1);
  }
}

static bool deklarasi_menunggu(const char *ptr) {
  Leksim t;
  while ((ptr = peleksim(ptr, &t)) && t.tipe != TIPE_LEKSIM_AKHIR) {
    if (t.tipe == TIPE_LEKSIM_KURUNG_BULAT_TUTUP)
      break;
    if (t.tipe == TIPE_LEKSIM_TIPE_DATA_UNTAIAN ||
        t.tipe == TIPE_LEKSIM_TIPE_DATA_BILANGAN) {
      return true;
    }
  }
  return false;
}

static const char *urai_argumen(const char *ptr, PSA *node) {
  Leksim lanjut;
  while ((ptr = peleksim(ptr, &lanjut)) &&
         lanjut.tipe != TIPE_LEKSIM_KURUNG_BULAT_TUTUP &&
         lanjut.tipe != TIPE_LEKSIM_AKHIR) {
    if (lanjut.tipe == TIPE_LEKSIM_PENGENAL) {
      tambah_anak(node, buat_node(PSA_PENGENAL, lanjut.teks, lanjut.panjang));
    } else if (lanjut.tipe == TIPE_LEKSIM_NILAI_UNTAIAN) {
      tambah_anak(node, buat_node(PSA_NILAI_UNTAIAN, lanjut.teks, lanjut.panjang));
    } else if (lanjut.tipe == TIPE_LEKSIM_NILAI_BILANGAN) {
      tambah_anak(node, buat_node(PSA_NILAI_BILANGAN, lanjut.teks, lanjut.panjang));
    } else if (lanjut.tipe == TIPE_LEKSIM_TIPE_DATA_UNTAIAN) {
      tambah_anak(node, buat_node(PSA_TIPE_DATA_UNTAIAN, lanjut.teks, lanjut.panjang));
    } else if (lanjut.tipe == TIPE_LEKSIM_TIPE_DATA_BILANGAN) {
      tambah_anak(node, buat_node(PSA_TIPE_DATA_BILANGAN, lanjut.teks, lanjut.panjang));
    } else if (lanjut.tipe == TIPE_LEKSIM_KATA_KUNCI_EKSTERNAL) {
      tambah_anak(node, buat_node(PSA_KATA_KUNCI, lanjut.teks, lanjut.panjang));
    } else if (lanjut.tipe == TIPE_LEKSIM_KATA_KUNCI_PUBLIK) {
      tambah_anak(node, buat_node(PSA_KATA_KUNCI_PUBLIK, lanjut.teks, lanjut.panjang));
    }
  }
  return ptr;
}

static PSA *urai_berkas(const char *isi, const char *jalur, bool titik_masuk);

static void muat_siswa(PSA *akar, PSA *node_muat, const char *teks_jalur,
                       int panjang) {
  char quote = teks_jalur[0];
  if (panjang < 2 || (quote != '\'' && quote != '"') ||
      teks_jalur[panjang - 1] != quote)
    return;

  int pjg_jalur = panjang - 2;
  if (pjg_jalur <= 0)
    return;

  char *nama = malloc((size_t)pjg_jalur + 1);
  if (!nama)
    return;
  strncpy(nama, teks_jalur + 1, pjg_jalur);
  nama[pjg_jalur] = '\0';

  char *res = NULL;
  cari_berkas(nama, &res, NULL);
  free(nama);
  if (!res)
    return;

  node_muat->jalur = salin(res);

  if (berkas_sudah_dimuat(res)) {
    for (int i = 0; i < akar->jumlah_anak; i++) {
      PSA *b = akar->anak[i];
      if (b->tipe == PSA_BERKAS && b->jalur && strcmp(b->jalur, res) == 0 && b->modul) {
        tambah_anak(node_muat, buat_node(PSA_PENGENAL, b->modul, (int)strlen(b->modul)));
      }
    }
    free(res);
    return;
  }

  tandai_berkas_dimuat(res);

  const char *sub_isi = baca_berkas(res);
  if (sub_isi) {
    PSA *sub = urai_berkas(sub_isi, res, false);
    for (int i = 0; i < sub->jumlah_anak; i++) {
      tambah_anak(akar, sub->anak[i]);
    }
    free(sub->anak);
    free(sub);
    bersihkan_berkas(sub_isi);

    for (int i = 0; i < akar->jumlah_anak; i++) {
      PSA *b = akar->anak[i];
      if (b->tipe == PSA_BERKAS && b->modul) {
        bool sudah_ada = false;
        for (int k = 1; k < node_muat->jumlah_anak; k++) {
          if (node_muat->anak[k]->tipe == PSA_PENGENAL &&
              strcmp(node_muat->anak[k]->teks, b->modul) == 0) {
            sudah_ada = true;
            break;
          }
        }
        if (!sudah_ada && b->jalur && strcmp(b->jalur, res) == 0) {
          tambah_anak(node_muat, buat_node(PSA_PENGENAL, b->modul, (int)strlen(b->modul)));
        }
      }
    }
  }

  free(res);
}

static PSA *urai_berkas(const char *isi, const char *jalur, bool titik_masuk) {
  PSA *akar = buat_node(PSA_PROGRAM, "program", 7);
  PSA *modul = buat_node(PSA_BERKAS, NULL, 0);
  modul->jalur = salin(jalur);
  modul->titik_masuk = titik_masuk;
  modul->modul = modul_dari_jalur(jalur, akar_dasar);

  Leksim leksim;
  const char *ptr = isi;

  while ((ptr = peleksim(ptr, &leksim)) && leksim.tipe != TIPE_LEKSIM_AKHIR) {
    if (leksim.tipe == TIPE_LEKSIM_KOMENTAR)
      continue;

    if (leksim.tipe == TIPE_LEKSIM_TIPE_DATA_MUAT) {
      Leksim lanjut;
      const char *p = ptr;
      char *alias = NULL;
      int alias_panjang = 0;
      int jumlah = 0;

      for (;;) {
        const char *p1 = peleksim(p, &lanjut);
        if (lanjut.tipe == TIPE_LEKSIM_KOMENTAR) {
          p = p1;
          continue;
        }

        if (lanjut.tipe == TIPE_LEKSIM_PENGENAL) {
          Leksim t2;
          const char *p2 = peleksim(p1, &t2);
          while (t2.tipe == TIPE_LEKSIM_KOMENTAR)
            p2 = peleksim(p2, &t2);
          if (t2.tipe != TIPE_LEKSIM_NILAI_UNTAIAN)
            break;
          free(alias);
          alias = salin_n(lanjut.teks, lanjut.panjang);
          alias_panjang = lanjut.panjang;
          p = p1;
          continue;
        }

        if (lanjut.tipe == TIPE_LEKSIM_NILAI_UNTAIAN) {
          PSA *node = buat_node(PSA_MUAT, alias ? alias : "_",
                                alias ? alias_panjang : 1);
          node->modul = salin(modul->modul);
          tambah_anak(node, buat_node(PSA_NILAI_UNTAIAN, lanjut.teks, lanjut.panjang));
          muat_siswa(akar, node, lanjut.teks, lanjut.panjang);
          tambah_anak(modul, node);
          jumlah++;
          free(alias);
          alias = NULL;
          alias_panjang = 0;
          p = p1;
          continue;
        }
        break;
      }

      free(alias);
      ptr = p;
      if (!jumlah)
        printf(
          "Kesalahan: sintaks muat 'muat [alias] \"nama_berkas.ns\"' (di %s)\n", 
          modul->jalur ? modul->jalur : "?"
        );
      continue;
    }

    if (leksim.tipe == TIPE_LEKSIM_PENGENAL) {
      Leksim lanjut;
      const char *ptr_lanjut = peleksim(ptr, &lanjut);


      if (lanjut.tipe == TIPE_LEKSIM_TITIK) {
        Leksim sesudah;
        const char *ptr_sesudah = peleksim(ptr_lanjut, &sesudah);
        if (sesudah.tipe == TIPE_LEKSIM_PENGENAL) {
          Leksim buka;
          const char *ptr_buka = peleksim(ptr_sesudah, &buka);
          if (buka.tipe == TIPE_LEKSIM_KURUNG_BULAT_BUKA) {
            size_t pjg = (size_t)leksim.panjang + 1 + (size_t)sesudah.panjang;
            char *nama = malloc(pjg + 1);
            memcpy(nama, leksim.teks, leksim.panjang);
            nama[leksim.panjang] = '.';
            memcpy(nama + leksim.panjang + 1, sesudah.teks, sesudah.panjang);
            nama[pjg] = '\0';

            PSA *node = buat_node(PSA_PANGGILAN, nama, (int)pjg);
            node->modul = salin(modul->modul);
            free(nama);
            tambah_anak(modul, node);
            ptr = urai_argumen(ptr_buka, node);
            continue;
          }
          printf(
            "Kesalahan: bertitik hanya didukung dalam panggilan, '%.*s.%.*s' (di %s)\n",
            leksim.panjang, leksim.teks, sesudah.panjang, sesudah.teks,
            modul->jalur ? modul->jalur : "?"
          );
          ptr = ptr_sesudah;
          continue;
        }
      }

      if (lanjut.tipe == TIPE_LEKSIM_KURUNG_BULAT_BUKA) {
        ptr = ptr_lanjut;

        Leksim lanjut2;
        const char *ptr_lanjut2 = peleksim(ptr, &lanjut2);

        bool adalah_deklarasi = false;
        if (lanjut2.tipe == TIPE_LEKSIM_PENGENAL) {
          adalah_deklarasi = deklarasi_menunggu(ptr_lanjut2);
        }

        if (adalah_deklarasi) {
          PSA *node = buat_node(PSA_DEKLARASI, leksim.teks, leksim.panjang);
          node->modul = salin(modul->modul);
          ptr = ptr_lanjut;

          while ((ptr = peleksim(ptr, &lanjut)) &&
                 lanjut.tipe != TIPE_LEKSIM_KURUNG_BULAT_TUTUP &&
                 lanjut.tipe != TIPE_LEKSIM_AKHIR) {
            if (lanjut.tipe == TIPE_LEKSIM_PENGENAL) {
              tambah_anak(node,
                          buat_node(PSA_PENGENAL, lanjut.teks, lanjut.panjang));
            } else if (lanjut.tipe == TIPE_LEKSIM_TIPE_DATA_UNTAIAN) {
              tambah_anak(node, buat_node(PSA_TIPE_DATA_UNTAIAN, lanjut.teks,
                                          lanjut.panjang));
            } else if (lanjut.tipe == TIPE_LEKSIM_TIPE_DATA_BILANGAN) {
              tambah_anak(node, buat_node(PSA_TIPE_DATA_BILANGAN, lanjut.teks,
                                          lanjut.panjang));
            } else if (lanjut.tipe == TIPE_LEKSIM_KATA_KUNCI_EKSTERNAL) {
              tambah_anak(
                  node, buat_node(PSA_KATA_KUNCI, lanjut.teks, lanjut.panjang));
            }
          }

          if (lanjut.tipe == TIPE_LEKSIM_KURUNG_BULAT_TUTUP) {
            ptr = peleksim(ptr, &lanjut);

            if (lanjut.tipe == TIPE_LEKSIM_KATA_KUNCI_EKSTERNAL) {
              tambah_anak(
                  node, buat_node(PSA_KATA_KUNCI, lanjut.teks, lanjut.panjang));
            } else if (lanjut.tipe == TIPE_LEKSIM_KATA_KUNCI_PUBLIK) {
              tambah_anak(node, buat_node(PSA_KATA_KUNCI_PUBLIK, lanjut.teks,
                                          lanjut.panjang));
            }
          }

          tambah_anak(modul, node);

          Leksim next;
          const char *ptr_next = peleksim(ptr, &next);
          if (next.tipe == TIPE_LEKSIM_KURUNG_KURAWAT_BUKA) {
            ptr = ptr_next;

            const char *start = ptr;
            int depth = 1;
            const char *inner = ptr;
            Leksim t;
            while (depth > 0 && (inner = peleksim(inner, &t)) &&
                   t.tipe != TIPE_LEKSIM_AKHIR) {
              if (t.tipe == TIPE_LEKSIM_KURUNG_KURAWAT_BUKA)
                depth++;
              else if (t.tipe == TIPE_LEKSIM_KURUNG_KURAWAT_TUTUP)
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
        } else {
          PSA *node = buat_node(PSA_PANGGILAN, leksim.teks, leksim.panjang);
          node->modul = salin(modul->modul);
          tambah_anak(modul, node);
          ptr = urai_argumen(ptr_lanjut, node);
        }
      } else {
        PSA *node = buat_node(PSA_PENGENAL, leksim.teks, leksim.panjang);
        tambah_anak(modul, node);
      }
    } else if (leksim.tipe == TIPE_LEKSIM_KATA_KUNCI_EKSTERNAL) {
      tambah_anak(modul, buat_node(PSA_KATA_KUNCI, leksim.teks, leksim.panjang));
    } else if (leksim.tipe == TIPE_LEKSIM_KATA_KUNCI_PUBLIK) {
      tambah_anak(modul,
                  buat_node(PSA_KATA_KUNCI_PUBLIK, leksim.teks, leksim.panjang));
    } else if (leksim.tipe == TIPE_LEKSIM_TIPE_DATA_UNTAIAN) {
      tambah_anak(modul,
                  buat_node(PSA_TIPE_DATA_UNTAIAN, leksim.teks, leksim.panjang));
    } else if (leksim.tipe == TIPE_LEKSIM_TIPE_DATA_BILANGAN) {
      tambah_anak(modul,
                  buat_node(PSA_TIPE_DATA_BILANGAN, leksim.teks, leksim.panjang));
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