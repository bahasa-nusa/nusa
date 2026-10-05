#include "nusa/pengurai.h"
#include "nusa/penolek.h"
#include <stdio.h>
#include <stdlib.h>

static const char *pesan_kesalahan = NULL;

const char *pesan_urai() { return pesan_kesalahan; }

static PSA *buat_node(TipePSA tipe, const char *teks, int panjang) {
  PSA *node = malloc(sizeof(PSA));
  node->tipe = tipe;
  node->teks = teks;
  node->panjang = panjang;
  node->anak = NULL;
  node->jumlah_anak = 0;

  return node;
}

static void tambah_anak(PSA *induk, PSA *anak) {
  induk->jumlah_anak++;
  induk->anak = realloc(induk->anak, sizeof(PSA *) * induk->jumlah_anak);
  induk->anak[induk->jumlah_anak - 1] = anak;
}

void bersihkan_psa(PSA *node) {
  if (!node) return;

  for (int i = 0; i < node->jumlah_anak; i++) {
    bersihkan_psa(node->anak[i]);
  }

  free(node->anak);
  free(node);
}

void cetak_psa(const PSA *node, int indent) {
  if (!node) return;

  for (int i = 0; i < indent; i++) printf("  ");

  const char *nama_tipe[] = {"PROGRAM", "PANGGILAN", "PENGENAL", "NILAI UNTAIAN", "KATA KUNCI"};
  printf("[%s] %.*s\n", nama_tipe[node->tipe], node->panjang, node->teks);

  for (int i = 0; i < node->jumlah_anak; i++) {
    cetak_psa(node->anak[i], indent + 1);
  }
}

PSA *urai(const char *isi) {
  PSA *akar = buat_node(PSA_PROGRAM, "program", 7);
  Tolek tolek;
  const char *ptr = isi;

  while ((ptr = penolek(ptr, &tolek)) && tolek.tipe != TIPE_TOLEK_AKHIR) {
    if (tolek.tipe == TIPE_TOLEK_KOMENTAR) continue;

    if (tolek.tipe == TIPE_TOLEK_NILAI_UNTAIAN) {
      tambah_anak(akar, buat_node(PSA_NILAI_UNTAIAN, tolek.teks, tolek.panjang));

    } else if (tolek.tipe == TIPE_TOLEK_PENGENAL) {
      PSA *node = buat_node(PSA_PANGGILAN, tolek.teks, tolek.panjang);
      
      Tolek lanjut;
      const char *ptr_lanjut = penolek(ptr, &lanjut);

      if (lanjut.tipe == TIPE_TOLEK_KURUNG_BULAT_BUKA) {
        ptr = ptr_lanjut;

        while ((ptr = penolek(ptr, &lanjut)) && lanjut.tipe != TIPE_TOLEK_KURUNG_BULAT_TUTUP && lanjut.tipe != TIPE_TOLEK_AKHIR) {
          if (lanjut.tipe == TIPE_TOLEK_PENGENAL) {
            tambah_anak(node, buat_node(PSA_PENGENAL, lanjut.teks, lanjut.panjang));
          } else if (lanjut.tipe == TIPE_TOLEK_NILAI_UNTAIAN) {
            tambah_anak(node, buat_node(PSA_NILAI_UNTAIAN, lanjut.teks, lanjut.panjang));
          } else if (lanjut.tipe == TIPE_TOLEK_KATA_KUNCI_EKSTERNAL || lanjut.tipe == TIPE_TOLEK_KATA_KUNCI_UNTAIAN) {
            tambah_anak(node, buat_node(PSA_KATA_KUNCI, lanjut.teks, lanjut.panjang));
          }
        }
      }
      
      tambah_anak(akar, node);
    } else if (tolek.tipe == TIPE_TOLEK_KATA_KUNCI_EKSTERNAL || tolek.tipe == TIPE_TOLEK_KATA_KUNCI_UNTAIAN) {
        tambah_anak(akar, buat_node(PSA_KATA_KUNCI, tolek.teks, tolek.panjang));
    }
  }

  return akar;
}