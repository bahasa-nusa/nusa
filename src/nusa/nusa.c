#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "nusa/berkas.h"
#include "nusa/penolek.h"
#include "nusa/urai_arg.h"
#include "nusa/pengurai.h"
#include "nusa/pesemantik.h"

void cetak_info() {
  printf("Penggunaan: nusa [argumen] <berkas>\n\n");
  printf("Opsi:\n");
  printf("-v, --versi     Untuk melihat versi.\n");
  printf("-i, --info      Untuk melihat informasi penggunaan.\n");
}

static char *salin(const char *s) {
  if (!s) return NULL;

  size_t p = strlen(s) + 1;
  char *r = malloc(p);
  if (r) memcpy(r, s, p);

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
    if (tolek.tipe == TIPE_TOLEK_KOMENTAR) continue;

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
    while (cur->lanjut) cur = cur->lanjut;
    cur->lanjut = ib;
  }
}

static void cetak_tolek_terkumpul(void) {
  int nomor = 0;
  InfoBer *cur = daftar_info;
  while (cur) {
    nomor++;
    if (nomor > 1) printf("\n");
    printf("Berkas %d: %s%s\n", nomor, cur->jalur, cur->titik_masuk ? " (titik masuk)" : "");
    
    int max_len = 0;
    const char *ptr = cur->isi;
    Tolek tolek;
    while ((ptr = penolek(ptr, &tolek)) && tolek.tipe != TIPE_TOLEK_AKHIR) {
      if (tolek.panjang > max_len) max_len = tolek.panjang;
    }
    if (max_len < 22) max_len = 22;

    ptr = cur->isi;
    while ((ptr = penolek(ptr, &tolek)) && tolek.tipe != TIPE_TOLEK_AKHIR) {
      printf("%*.*s | %s\n", max_len, tolek.panjang, tolek.teks, nama_tolek(tolek.tipe));
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
    const char *isi_berkas = baca_berkas(arg.input_file);
    if (!isi_berkas) {
      printf("Gagal baca: %s\n", arg.input_file);
      return 1;
    }

    // Tolek
    printf("Tolek:\n");
    bersihkan_daftar_dimuat();
    tandai_berkas_dimuat(arg.input_file);
    kumpul_tolek(isi_berkas, arg.input_file, true);
    cetak_tolek_terkumpul();

    // PSA
    printf("PSA:\n");
    bersihkan_daftar_dimuat();
    tandai_berkas_dimuat(arg.input_file);

    PSA *psa = urai(isi_berkas, arg.input_file);
    if (psa) {
      cetak_psa(psa, 0);
      printf("\nPesemantik:\n");

      int galat = pesemantik(psa);
      if(!galat) printf("Tidak ada kesalahan\n");

      bersihkan_psa(psa);
    } else {
      printf("Gagal mengurai: %s\n", pesan_urai());
    }

    bersihkan_daftar_dimuat();
    bersihkan_berkas(isi_berkas);
  } else {
    printf("Argumen tidak valid.\n");
    cetak_info();
  }

  return 0;
}