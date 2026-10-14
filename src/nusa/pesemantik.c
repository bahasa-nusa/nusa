#include "nusa/pesemantik.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *nama_modul;
  char *jalur_berkas;
  const PSA *node_berkas;
} EntriModulTersedia;

typedef struct {
  EntriModulTersedia *entri;
  int jumlah;
  int kapasitas;
} TabelModulTersedia;

typedef struct {
  const PSA *node_berkas;
  char **nama_modul;
  int jumlah;
  int kapasitas;
} ModulDiimporBerkas;

typedef struct {
  ModulDiimporBerkas *daftar;
  int jumlah;
  int kapasitas;
} TabelModulDiimpor;

typedef struct {
  char *nama;
  char *modul;
  bool pub;
  bool eks;
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

static char *salin_teks_n(const char *s, int n) {
  if (!s)
    return NULL;
  char *r = malloc((size_t)n + 1);
  if (!r)
    return NULL;
  memcpy(r, s, (size_t)n);
  r[n] = '\0';
  return r;
}

static char *salin_teks(const char *s) {
  if (!s)
    return NULL;
  return salin_teks_n(s, (int)strlen(s));
}

static bool sama_string(const char *a, const char *b) {
  if (!a && !b)
    return true;
  if (!a || !b)
    return false;
  return strcmp(a, b) == 0;
}

static char *ekstrak_nama_modul(const char *jalur) {
  if (!jalur)
    return NULL;

  const char *mulai = jalur;
  if (strncmp(mulai, "./", 2) == 0 || strncmp(mulai, ".\\", 2) == 0)
    mulai += 2;

  size_t pjg = strlen(mulai);
  if (pjg >= 3 && strcmp(mulai + pjg - 3, ".ns") == 0)
    pjg -= 3;

  char *r = salin_teks_n(mulai, (int)pjg);
  if (!r)
    return NULL;

  for (char *c = r; *c; c++) {
    if (*c == '/' || *c == '\\')
      *c = '.';
  }

  return r;
}

static void daftarkan_modul_tersedia(TabelModulTersedia *tabel, const PSA *berkas) {
  char *nama = (berkas->modul && strlen(berkas->modul) > 0)
                   ? salin_teks(berkas->modul)
                   : ekstrak_nama_modul(berkas->jalur);
  if (!nama)
    return;

  if (tabel->jumlah >= tabel->kapasitas) {
    tabel->kapasitas = tabel->kapasitas == 0 ? 8 : tabel->kapasitas * 2;
    tabel->entri = realloc(tabel->entri, sizeof(EntriModulTersedia) * (size_t)tabel->kapasitas);
  }

  tabel->entri[tabel->jumlah].nama_modul = nama;
  tabel->entri[tabel->jumlah].jalur_berkas = berkas->jalur ? salin_teks(berkas->jalur) : NULL;
  tabel->entri[tabel->jumlah].node_berkas = berkas;
  tabel->jumlah++;
}

static bool modul_tersedia(const TabelModulTersedia *tabel, const char *nama) {
  if (!nama)
    return false;
  for (int i = 0; i < tabel->jumlah; i++) {
    if (strcmp(tabel->entri[i].nama_modul, nama) == 0)
      return true;
  }
  return false;
}

static void daftarkan_modul_diimpor(TabelModulDiimpor *tabel, const PSA *berkas, const char *nama_modul) {
  ModulDiimporBerkas *entri = NULL;
  for (int i = 0; i < tabel->jumlah; i++) {
    if (tabel->daftar[i].node_berkas == berkas) {
      entri = &tabel->daftar[i];
      break;
    }
  }

  if (!entri) {
    if (tabel->jumlah >= tabel->kapasitas) {
      tabel->kapasitas = tabel->kapasitas == 0 ? 8 : tabel->kapasitas * 2;
      tabel->daftar = realloc(tabel->daftar, sizeof(ModulDiimporBerkas) * (size_t)tabel->kapasitas);
    }
    entri = &tabel->daftar[tabel->jumlah];
    entri->node_berkas = berkas;
    entri->nama_modul = NULL;
    entri->jumlah = 0;
    entri->kapasitas = 0;
    tabel->jumlah++;
  }

  for (int i = 0; i < entri->jumlah; i++) {
    if (strcmp(entri->nama_modul[i], nama_modul) == 0)
      return;
  }

  if (entri->jumlah >= entri->kapasitas) {
    entri->kapasitas = entri->kapasitas == 0 ? 4 : entri->kapasitas * 2;
    entri->nama_modul = realloc(entri->nama_modul, sizeof(char *) * (size_t)entri->kapasitas);
  }

  entri->nama_modul[entri->jumlah] = salin_teks(nama_modul);
  entri->jumlah++;
}

static bool modul_diizinkan(const TabelModulDiimpor *tabel, const PSA *berkas, const char *target_modul) {
  if (!target_modul)
    return true;
  if (berkas->modul && strcmp(berkas->modul, target_modul) == 0)
    return true;

  for (int i = 0; i < tabel->jumlah; i++) {
    if (tabel->daftar[i].node_berkas == berkas) {
      for (int j = 0; j < tabel->daftar[i].jumlah; j++) {
        if (strcmp(tabel->daftar[i].nama_modul[j], target_modul) == 0)
          return true;
      }
      return false;
    }
  }
  return false;
}

static bool berkas_meminta_modul(const PSA *berkas, const char *nama_modul) {
  if (!berkas || !nama_modul)
    return false;
  for (int i = 0; i < berkas->jumlah_anak; i++) {
    const PSA *item = berkas->anak[i];
    if (item->tipe != PSA_MOD)
      continue;
    for (int j = 0; j < item->jumlah_anak; j++) {
      if (item->anak[j]->tipe == PSA_PENGENAL &&
          strcmp(item->anak[j]->teks, nama_modul) == 0)
        return true;
    }
  }
  return false;
}

static bool daftarkan_fungsi(TabelFungsi *tabel, const PSA *deklarasi, const char *modul) {
  for (int i = 0; i < tabel->jumlah; i++) {
    if (strcmp(tabel->data[i].nama, deklarasi->teks) == 0 &&
        sama_string(tabel->data[i].modul, modul))
      return false;
  }

  if (tabel->jumlah >= tabel->kapasitas) {
    tabel->kapasitas = tabel->kapasitas == 0 ? 16 : tabel->kapasitas * 2;
    tabel->data = realloc(tabel->data, sizeof(Fungsi) * (size_t)tabel->kapasitas);
  }

  Fungsi *fn = &tabel->data[tabel->jumlah];
  tabel->jumlah++;

  fn->nama = salin_teks(deklarasi->teks);
  fn->modul = modul ? salin_teks(modul) : NULL;
  fn->pub = false;
  fn->eks = false;
  fn->jumlah_parameter = 0;

  int kapasitas_param = deklarasi->jumlah_anak > 0 ? deklarasi->jumlah_anak : 1;
  fn->nama_parameter = malloc(sizeof(char *) * (size_t)kapasitas_param);
  fn->tipe_parameter = malloc(sizeof(TipePSA) * (size_t)kapasitas_param);
  fn->bertipe = malloc(sizeof(bool) * (size_t)kapasitas_param);

  for (int i = 0; i < deklarasi->jumlah_anak; i++) {
    const PSA *anak = deklarasi->anak[i];

    if (anak->tipe == PSA_KATA_KUNCI) {
      fn->eks = true;
      fn->pub = true;
      continue;
    }
    if (anak->tipe == PSA_KATA_KUNCI_PUBLIK) {
      fn->pub = true;
      continue;
    }
    if (anak->tipe == PSA_PANGGILAN || anak->tipe == PSA_DEKLARASI) {
      break;
    }
    if (anak->tipe == PSA_PENGENAL) {
      if (i + 1 < deklarasi->jumlah_anak &&
          (deklarasi->anak[i + 1]->tipe == PSA_TIPE_DATA_BILANGAN ||
           deklarasi->anak[i + 1]->tipe == PSA_TIPE_DATA_UNTAIAN)) {
        fn->nama_parameter[fn->jumlah_parameter] = salin_teks(anak->teks);
        fn->tipe_parameter[fn->jumlah_parameter] = deklarasi->anak[i + 1]->tipe;
        fn->bertipe[fn->jumlah_parameter] = true;
        fn->jumlah_parameter++;
        i++;
      }
    }
  }

  return true;
}

static Fungsi *cari_fungsi(const TabelFungsi *tabel, const char *nama, const char *modul) {
  for (int i = 0; i < tabel->jumlah; i++) {
    if (strcmp(tabel->data[i].nama, nama) == 0 &&
        sama_string(tabel->data[i].modul, modul))
      return &tabel->data[i];
  }
  return NULL;
}

static const char *nama_tipe_semantik(TipePSA tipe) {
  switch (tipe) {
  case PSA_TIPE_DATA_BILANGAN:
    return "b32";
  case PSA_TIPE_DATA_UNTAIAN:
    return "unt";
  case PSA_NILAI_BILANGAN:
    return "bilangan";
  case PSA_NILAI_UNTAIAN:
    return "untaian";
  default:
    return "lain";
  }
}

static int cek_satu_panggilan(const PSA *panggilan, const PSA *berkas,
                              const TabelModulTersedia *tabel_modul,
                              const TabelModulDiimpor *tabel_diimpor,
                              const TabelFungsi *tabel_fungsi) {
  const char *jalur = berkas->jalur ? berkas->jalur : "?";
  const char *titik = strrchr(panggilan->teks, '.');

  if (titik) {
    size_t pjg_modul = (size_t)(titik - panggilan->teks);
    char *nama_modul = salin_teks_n(panggilan->teks, (int)pjg_modul);
    const char *nama_fungsi = titik + 1;

    if (!modul_diizinkan(tabel_diimpor, berkas, nama_modul)) {
      if (berkas_meminta_modul(berkas, nama_modul)) {
        free(nama_modul);
        return 1;
      }
      if (!modul_tersedia(tabel_modul, nama_modul)) {
        fprintf(stderr, "Kesalahan: modul '%s' belum dimuat (di %s)\n",
                nama_modul, jalur);
      } else {
        fprintf(stderr, "Kesalahan: modul '%s' belum diimpor (di %s)\n",
                nama_modul, jalur);
      }
      free(nama_modul);
      return 1;
    }

    Fungsi *fn = cari_fungsi(tabel_fungsi, nama_fungsi, nama_modul);
    if (!fn) {
      fprintf(stderr, "Kesalahan: fungsi '%s' tidak dideklarasikan (di %s)\n",
              panggilan->teks, jalur);
      free(nama_modul);
      return 1;
    }

    if (!sama_string(berkas->modul, nama_modul) && !fn->pub && !fn->eks) {
      fprintf(stderr,
              "Kesalahan: fungsi '%s' pada modul '%s' bersifat privat dan tidak "
              "dapat diakses (di %s)\n",
              fn->nama, fn->modul ? fn->modul : "?", jalur);
      free(nama_modul);
      return 1;
    }

    int jml_argumen = panggilan->jumlah_anak;
    while (jml_argumen > 0 &&
           (panggilan->anak[jml_argumen - 1]->tipe == PSA_KATA_KUNCI ||
            panggilan->anak[jml_argumen - 1]->tipe == PSA_KATA_KUNCI_PUBLIK)) {
      jml_argumen--;
    }

    if (jml_argumen != fn->jumlah_parameter) {
      fprintf(stderr,
              "Kesalahan: '%s' deklarasikan %d parameter, dipanggil dengan %d "
              "argumen (di %s)\n",
              fn->nama, fn->jumlah_parameter, jml_argumen, jalur);
      free(nama_modul);
      return 1;
    }

    int galat = 0;
    for (int i = 0; i < jml_argumen; i++) {
      TipePSA tipe_param = fn->tipe_parameter[i];
      if (!fn->bertipe[i])
        continue;

      bool cocok = (tipe_param == PSA_TIPE_DATA_BILANGAN &&
                    panggilan->anak[i]->tipe == PSA_NILAI_BILANGAN) ||
                   (tipe_param == PSA_TIPE_DATA_UNTAIAN &&
                    panggilan->anak[i]->tipe == PSA_NILAI_UNTAIAN);

      if (!cocok) {
        fprintf(stderr,
                "Kesalahan: parameter %d '%s' di '%s' Tl. %s, bukan %s (di %s)\n",
                i + 1, fn->nama_parameter[i], fn->nama,
                nama_tipe_semantik(tipe_param),
                nama_tipe_semantik(panggilan->anak[i]->tipe), jalur);
        galat++;
      }
    }

    free(nama_modul);
    return galat;
  }

  Fungsi *fn = cari_fungsi(tabel_fungsi, panggilan->teks, berkas->modul);
  if (!fn) {
    Fungsi *lain = NULL;
    for (int i = 0; i < tabel_fungsi->jumlah; i++) {
      if (strcmp(tabel_fungsi->data[i].nama, panggilan->teks) == 0) {
        lain = &tabel_fungsi->data[i];
        break;
      }
    }
    if (lain) {
      if (!lain->pub && !lain->eks) {
        fprintf(stderr,
                "Kesalahan: fungsi '%s' pada modul '%s' bersifat privat dan tidak "
                "dapat diakses (di %s)\n",
                lain->nama, lain->modul ? lain->modul : "?", jalur);
      } else {
        fprintf(stderr,
                "Kesalahan: fungsi '%s' dari modul '%s' harus dipanggil dengan "
                "kualifikasi modul (di %s)\n",
                lain->nama, lain->modul ? lain->modul : "?", jalur);
      }
      return 1;
    }

    fprintf(stderr, "Kesalahan: fungsi '%s' tidak dideklarasikan (di %s)\n",
            panggilan->teks, jalur);
    return 1;
  }

  int jml_argumen = panggilan->jumlah_anak;
  while (jml_argumen > 0 &&
         (panggilan->anak[jml_argumen - 1]->tipe == PSA_KATA_KUNCI ||
          panggilan->anak[jml_argumen - 1]->tipe == PSA_KATA_KUNCI_PUBLIK)) {
    jml_argumen--;
  }

  if (jml_argumen != fn->jumlah_parameter) {
    fprintf(stderr,
            "Kesalahan: '%s' deklarasikan %d parameter, dipanggil dengan %d "
            "argumen (di %s)\n",
            fn->nama, fn->jumlah_parameter, jml_argumen, jalur);
    return 1;
  }

  int galat = 0;
  for (int i = 0; i < jml_argumen; i++) {
    TipePSA tipe_param = fn->tipe_parameter[i];
    if (!fn->bertipe[i])
      continue;

    bool cocok = (tipe_param == PSA_TIPE_DATA_BILANGAN &&
                  panggilan->anak[i]->tipe == PSA_NILAI_BILANGAN) ||
                 (tipe_param == PSA_TIPE_DATA_UNTAIAN &&
                  panggilan->anak[i]->tipe == PSA_NILAI_UNTAIAN);

    if (!cocok) {
      fprintf(stderr,
              "Kesalahan: parameter %d '%s' di '%s' Tl. %s, bukan %s (di %s)\n",
              i + 1, fn->nama_parameter[i], fn->nama,
              nama_tipe_semantik(tipe_param),
              nama_tipe_semantik(panggilan->anak[i]->tipe), jalur);
      galat++;
    }
  }

  return galat;
}

static int periksa_panggilan_rekursif(const PSA *node, const PSA *berkas,
                                      const TabelModulTersedia *tabel_modul,
                                      const TabelModulDiimpor *tabel_diimpor,
                                      const TabelFungsi *tabel_fungsi) {
  int galat = 0;
  if (!node)
    return 0;

  if (node->tipe == PSA_PANGGILAN) {
    galat += cek_satu_panggilan(node, berkas, tabel_modul, tabel_diimpor, tabel_fungsi);
  }

  for (int i = 0; i < node->jumlah_anak; i++) {
    galat += periksa_panggilan_rekursif(node->anak[i], berkas, tabel_modul, tabel_diimpor, tabel_fungsi);
  }

  return galat;
}

static void bersihkan_tabel_tersedia(TabelModulTersedia *t) {
  for (int i = 0; i < t->jumlah; i++) {
    free(t->entri[i].nama_modul);
    free(t->entri[i].jalur_berkas);
  }
  free(t->entri);
}

static void bersihkan_tabel_diimpor(TabelModulDiimpor *t) {
  for (int i = 0; i < t->jumlah; i++) {
    for (int j = 0; j < t->daftar[i].jumlah; j++) {
      free(t->daftar[i].nama_modul[j]);
    }
    free(t->daftar[i].nama_modul);
  }
  free(t->daftar);
}

static void bersihkan_tabel_fungsi(TabelFungsi *t) {
  for (int i = 0; i < t->jumlah; i++) {
    free(t->data[i].nama);
    free(t->data[i].modul);
    for (int j = 0; j < t->data[i].jumlah_parameter; j++) {
      free(t->data[i].nama_parameter[j]);
    }
    free(t->data[i].nama_parameter);
    free(t->data[i].tipe_parameter);
    free(t->data[i].bertipe);
  }
  free(t->data);
}

int pesemantik(const PSA *akar) {
  if (!akar)
    return 0;

  int jumlah_berkas = 0;
  const PSA **daftar_berkas = NULL;

  if (akar->tipe == PSA_BERKAS) {
    jumlah_berkas = 1;
    daftar_berkas = malloc(sizeof(PSA *));
    daftar_berkas[0] = akar;
  } else if (akar->tipe == PSA_PROGRAM) {
    daftar_berkas = malloc(sizeof(PSA *) * (size_t)(akar->jumlah_anak > 0 ? akar->jumlah_anak : 1));
    for (int i = 0; i < akar->jumlah_anak; i++) {
      if (akar->anak[i]->tipe == PSA_BERKAS) {
        daftar_berkas[jumlah_berkas++] = akar->anak[i];
      }
    }
  } else {
    return 0;
  }

  int galat = 0;
  TabelModulTersedia tabel_tersedia = {0};
  TabelModulDiimpor tabel_diimpor = {0};
  TabelFungsi tabel_fungsi = {0};

  for (int i = 0; i < jumlah_berkas; i++) {
    daftarkan_modul_tersedia(&tabel_tersedia, daftar_berkas[i]);
  }

  for (int i = 0; i < jumlah_berkas; i++) {
    const PSA *berkas = daftar_berkas[i];
    for (int j = 0; j < berkas->jumlah_anak; j++) {
      const PSA *item = berkas->anak[j];
      if (item->tipe != PSA_MOD)
        continue;

      for (int k = 0; k < item->jumlah_anak; k++) {
        const PSA *mod_anak = item->anak[k];
        if (mod_anak->tipe == PSA_PENGENAL) {
          if (!modul_tersedia(&tabel_tersedia, mod_anak->teks)) {
            fprintf(stderr, "Kesalahan: modul '%s' belum dimuat (di %s)\n",
                    mod_anak->teks, berkas->jalur ? berkas->jalur : "?");
            galat++;
          } else {
            daftarkan_modul_diimpor(&tabel_diimpor, berkas, mod_anak->teks);
          }
        }
      }
    }
  }

  for (int i = 0; i < jumlah_berkas; i++) {
    const PSA *berkas = daftar_berkas[i];
    for (int j = 0; j < berkas->jumlah_anak; j++) {
      const PSA *item = berkas->anak[j];
      if (item->tipe != PSA_DEKLARASI)
        continue;

      if (!daftarkan_fungsi(&tabel_fungsi, item, berkas->modul)) {
        fprintf(stderr,
                "Kesalahan: '%s' dideklarasikan lebih dari sekali di modul '%s'\n",
                item->teks, berkas->modul ? berkas->modul : "?");
        galat++;
      }
    }
  }

  for (int i = 0; i < jumlah_berkas; i++) {
    const PSA *berkas = daftar_berkas[i];
    galat += periksa_panggilan_rekursif(berkas, berkas, &tabel_tersedia,
                                        &tabel_diimpor, &tabel_fungsi);
  }

  free(daftar_berkas);
  bersihkan_tabel_tersedia(&tabel_tersedia);
  bersihkan_tabel_diimpor(&tabel_diimpor);
  bersihkan_tabel_fungsi(&tabel_fungsi);

  return galat;
}