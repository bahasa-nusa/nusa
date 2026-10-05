#include "nusa/pesemantik.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *nama;
  int jumlah_parameter;
  char **nama_parameter;
  TipePSA *tipe_parameter;
  bool *bertipe;
} Fungsi;

typedef struct {
  Fungsi *data;
  int jumlah;
  int kapasitas;
} TabelFungsi;

static TabelFungsi tabel;

static Fungsi *cari_fungsi(const char *nama) {
  for (int i = 0; i < tabel.jumlah; i++) {
    if (strcmp(tabel.data[i].nama, nama) == 0) return &tabel.data[i];
  }

  return NULL;
}

static bool daftarkan(const PSA *deklarasi) {
  int kapasitas = deklarasi->jumlah_anak > 0 ? deklarasi->jumlah_anak : 1;

  if (cari_fungsi(deklarasi->teks)) return false;

  tabel.data = realloc(tabel.data, sizeof(Fungsi) * (tabel.jumlah + 1));
  Fungsi *fn = &tabel.data[tabel.jumlah];
  tabel.jumlah++;

  fn->nama = malloc(strlen(deklarasi->teks) + 1);
  strcpy(fn->nama, deklarasi->teks);
  fn->jumlah_parameter = 0;
  fn->nama_parameter = malloc(sizeof(char *) * kapasitas);
  fn->tipe_parameter = malloc(sizeof(TipePSA) * kapasitas);
  fn->bertipe = malloc(sizeof(bool) * kapasitas);

  for (int i = 0; i < deklarasi->jumlah_anak; i++) {
    const PSA *anak = deklarasi->anak[i];

    if (anak->tipe == PSA_PENGENAL) {
      fn->nama_parameter[fn->jumlah_parameter] = malloc(strlen(anak->teks) + 1);
      strcpy(fn->nama_parameter[fn->jumlah_parameter], anak->teks);
      fn->tipe_parameter[fn->jumlah_parameter] = (TipePSA)0;
      fn->bertipe[fn->jumlah_parameter] = false;
      fn->jumlah_parameter++;

    } else if (anak->tipe == PSA_TIPE_DATA_BILANGAN || anak->tipe == PSA_TIPE_DATA_UNTAIAN) {
      if (fn->jumlah_parameter == 0) continue;
      fn->tipe_parameter[fn->jumlah_parameter - 1] = anak->tipe;
      fn->bertipe[fn->jumlah_parameter - 1] = true;
    }
  }

  return true;
}

static const char *nama_tipe_semantik(TipePSA tipe) {
  switch (tipe) {
    case PSA_TIPE_DATA_BILANGAN: return "b32";
    case PSA_TIPE_DATA_UNTAIAN: return "unt";
    case PSA_NILAI_BILANGAN: return "bilangan";
    case PSA_NILAI_UNTAIAN: return "untaian";
    default: return "lain";
  }
}

static int cek_panggilan(const PSA *panggilan, const char *jalur) {
  Fungsi *fn = cari_fungsi(panggilan->teks);

  if (!fn) {
    printf("Kesalahan: fungsi '%s' tidak dideklarasikan (di %s)\n", panggilan->teks, jalur);
    return 1;
  }

  int jml_argumen = panggilan->jumlah_anak;
  while (jml_argumen > 0 && panggilan->anak[jml_argumen - 1]->tipe == PSA_KATA_KUNCI) {
    jml_argumen--;
  }

  if (jml_argumen != fn->jumlah_parameter) {
    printf("Kesalahan: '%s' deklarasikan %d parameter, dipanggil dengan %d argumen (di %s)\n",
      fn->nama, fn->jumlah_parameter, jml_argumen, jalur);
    return 1;
  }

  int galat = 0;
  for (int i = 0; i < jml_argumen; i++) {
    TipePSA tipe_param = fn->tipe_parameter[i];

    if (!fn->bertipe[i]) continue;

    bool cocok = (tipe_param == PSA_TIPE_DATA_BILANGAN && panggilan->anak[i]->tipe == PSA_NILAI_BILANGAN)
              || (tipe_param == PSA_TIPE_DATA_UNTAIAN && panggilan->anak[i]->tipe == PSA_NILAI_UNTAIAN);

    if (!cocok) {
      printf("Kesalahan: parameter %d '%s' di '%s' Tl. %s, bukan %s (di %s)\n",
        i + 1, fn->nama_parameter[i], fn->nama,
        nama_tipe_semantik(tipe_param), nama_tipe_semantik(panggilan->anak[i]->tipe), jalur);
      galat++;
    }
  }

  return galat;
}

static void bersihkan_tabel(void) {
  for (int i = 0; i < tabel.jumlah; i++) {
    Fungsi *fn = &tabel.data[i];
    for (int j = 0; j < fn->jumlah_parameter; j++) free(fn->nama_parameter[j]);

    free(fn->nama_parameter);
    free(fn->tipe_parameter);
    free(fn->bertipe);
    free(fn->nama);
  }

  free(tabel.data);
  memset(&tabel, 0, sizeof(tabel));
}

int pesemantik(const PSA *akar) {
  int galat = 0;

  for (int i = 0; i < akar->jumlah_anak; i++) {
    const PSA *berkas = akar->anak[i];
    if (berkas->tipe != PSA_BERKAS) continue;

    for (int j = 0; j < berkas->jumlah_anak; j++) {
      const PSA *item = berkas->anak[j];
      if (item->tipe != PSA_DEKLARASI) continue;

      if (!daftarkan(item)) {
        printf("Kesalahan: '%s' dideklarasikan lebih dari sekali\n", item->teks);
        galat++;
      }
    }
  }

  for (int i = 0; i < akar->jumlah_anak; i++) {
    const PSA *berkas = akar->anak[i];
    if (berkas->tipe != PSA_BERKAS) continue;

    for (int j = 0; j < berkas->jumlah_anak; j++) {
      const PSA *item = berkas->anak[j];
      if (item->tipe == PSA_PANGGILAN) galat += cek_panggilan(item, berkas->jalur);
    }
  }

  bersihkan_tabel();
  return galat;
}