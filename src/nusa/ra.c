// Copyright 2026 Pengembang Bahasa Pemrograman Nusa
// SPDX-License-Identifier: Apache-2.0

#include "nusa/ra.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *salin(const char *s, int n) {
  char *r = malloc((size_t)n + 1);
  memcpy(r, s, (size_t)n);
  r[n] = '\0';
  return r;
}

static void dorong(InstruksiRA **kepala, InstruksiRA **ekor, InstruksiRA *ins) {
  ins->next = NULL;

  if (*kepala == NULL) {
    *kepala = *ekor = ins;
    return;
  }
  (*ekor)->next = ins;
  *ekor = ins;
}

static TipeNilaiRA tipe_dari_psa(TipePSA tipe) {
  if (tipe == PSA_TIPE_DATA_BILANGAN)
    return RA_BILANGAN;
  if (tipe == PSA_TIPE_DATA_UNTAIAN)
    return RA_UNTAIAN;
  return RA_TANPA_TIPE;
}

static void isi_dari_deklarasi(InstruksiRA *ins, const PSA *deklarasi) {
  ins->tipe = RA_FUNGSI;
  ins->nama = salin(deklarasi->teks, deklarasi->panjang);
  ins->jumlah = 0;
  ins->tipe_kembali = RA_TANPA_TIPE;
  ins->eks = false;
  ins->pub = false;
  ins->modul = NULL;
  ins->nilai = NULL;
  ins->tipe_nilai = NULL;

  int kapasitas = deklarasi->jumlah_anak > 0 ? deklarasi->jumlah_anak : 1;
  ins->nilai = malloc(sizeof(char *) * kapasitas);
  ins->tipe_nilai = malloc(sizeof(TipeNilaiRA) * kapasitas);

  bool menunggu_tipe_param = false;
  for (int i = 0; i < deklarasi->jumlah_anak; i++) {
    const PSA *anak = deklarasi->anak[i];
    if (anak->tipe == PSA_KATA_KUNCI) {
      ins->eks = true;
      menunggu_tipe_param = false;
      continue;
    }
    if (anak->tipe == PSA_KATA_KUNCI_PUBLIK) {
      ins->pub = true;
      menunggu_tipe_param = false;
      continue;
    }
    if (anak->tipe == PSA_TIPE_DATA_BILANGAN) {
      if (menunggu_tipe_param) {
        ins->tipe_nilai[ins->jumlah - 1] = RA_BILANGAN;
        menunggu_tipe_param = false;
      } else {
        ins->tipe_kembali = RA_BILANGAN;
      }
      continue;
    }
    if (anak->tipe == PSA_TIPE_DATA_UNTAIAN) {
      if (menunggu_tipe_param) {
        ins->tipe_nilai[ins->jumlah - 1] = RA_UNTAIAN;
        menunggu_tipe_param = false;
      } else {
        ins->tipe_kembali = RA_UNTAIAN;
      }
      continue;
    }
    if (anak->tipe != PSA_PENGENAL)
      continue;

    ins->nilai[ins->jumlah] = salin(anak->teks, anak->panjang);
    ins->tipe_nilai[ins->jumlah] = RA_TANPA_TIPE;
    ins->jumlah++;
    menunggu_tipe_param = true;
  }
}

static void isi_dari_panggilan(InstruksiRA *ins, const PSA *panggilan) {
  ins->tipe = RA_PANGGIL;
  ins->nama = salin(panggilan->teks, panggilan->panjang);
  ins->jumlah = 0;
  ins->eks = false;
  ins->pub = false;
  ins->modul = NULL;
  ins->nilai = NULL;
  ins->tipe_nilai = NULL;

  int kapasitas = panggilan->jumlah_anak > 0 ? panggilan->jumlah_anak : 1;
  ins->nilai = malloc(sizeof(char *) * kapasitas);
  ins->tipe_nilai = malloc(sizeof(TipeNilaiRA) * kapasitas);

  for (int i = 0; i < panggilan->jumlah_anak; i++) {
    const PSA *argumen = panggilan->anak[i];
    if (argumen->tipe == PSA_KATA_KUNCI ||
        argumen->tipe == PSA_KATA_KUNCI_PUBLIK)
      continue;

    ins->nilai[ins->jumlah] = salin(argumen->teks, argumen->panjang);

    TipeNilaiRA tipe = RA_TANPA_TIPE;
    if (argumen->tipe == PSA_NILAI_BILANGAN)
      tipe = RA_BILANGAN;
    else if (argumen->tipe == PSA_NILAI_UNTAIAN)
      tipe = RA_UNTAIAN;
    ins->tipe_nilai[ins->jumlah] = tipe;
    ins->jumlah++;
  }
}

static char *salin_modul(const char *m) {
  if (!m)
    return NULL;
  return salin(m, (int)strlen(m));
}

static InstruksiRA *buat_titik_masuk(void) {
  InstruksiRA *ins = calloc(1, sizeof(InstruksiRA));
  ins->tipe = RA_FUNGSI;
  ins->jumlah = 0;
  ins->eks = false;
  ins->pub = true;
  ins->modul = NULL;
  ins->nama = salin("titik_masuk", 11);
  ins->nilai = malloc(sizeof(char *));
  ins->tipe_nilai = malloc(sizeof(TipeNilaiRA));
  ins->badan = NULL;

  return ins;
}

typedef struct {
  const InstruksiRA *deklarasi;
  const char *modul;
  bool publik;
} Simbol;

static Simbol *tabel_simbol;
static int jumlah_simbol;

typedef struct {
  char *var;
  char *modul;
} ModulMuat;
static ModulMuat *modul_muat;
static int jumlah_modul_muat;

static void kumpul_modul_muat(const PSA *akar) {
  for (int i = 0; i < akar->jumlah_anak; i++) {
    const PSA *berkas = akar->anak[i];
    if (berkas->tipe != PSA_BERKAS)
      continue;
    for (int j = 0; j < berkas->jumlah_anak; j++) {
      const PSA *muat = berkas->anak[j];
      if (muat->tipe != PSA_MUAT)
        continue;
      for (int k = 1; k < muat->jumlah_anak; k++) {
        if (muat->anak[k]->tipe != PSA_PENGENAL)
          continue;
        jumlah_modul_muat++;
        modul_muat = realloc(modul_muat, sizeof(ModulMuat) * jumlah_modul_muat);
        modul_muat[jumlah_modul_muat - 1].var =
            salin(muat->teks, (int)strlen(muat->teks));
        modul_muat[jumlah_modul_muat - 1].modul =
            salin(muat->anak[k]->teks, (int)strlen(muat->anak[k]->teks));
      }
    }
  }
}

static const char *modul_dari_var(const char *var) {
  for (int i = 0; i < jumlah_modul_muat; i++)
    if (strcmp(modul_muat[i].var, var) == 0)
      return modul_muat[i].modul;
  return NULL;
}

InstruksiRA *tambah_impor_ra(InstruksiRA *daftar, const InstruksiRA *sumber);

static void kumpulkan_simbol(const InstruksiRA *daftar) {
  for (const InstruksiRA *cur = daftar; cur; cur = cur->next) {
    if (cur->tipe != RA_FUNGSI)
      continue;

    jumlah_simbol++;
    tabel_simbol = realloc(tabel_simbol, sizeof(Simbol) * jumlah_simbol);
    tabel_simbol[jumlah_simbol - 1].deklarasi = cur;
    tabel_simbol[jumlah_simbol - 1].modul = cur->modul;
    tabel_simbol[jumlah_simbol - 1].publik = cur->pub || cur->eks;
  }
}

static bool modul_sama_ins(const InstruksiRA *ins, const char *m) {
  if (!ins->modul && !m)
    return true;
  if (!ins->modul || !m)
    return false;
  return strcmp(ins->modul, m) == 0;
}

static const InstruksiRA *cari_simbol(const InstruksiRA *panggil) {
  for (int i = 0; i < jumlah_simbol; i++) {
    const InstruksiRA *d = tabel_simbol[i].deklarasi;
    if (strcmp(d->nama, panggil->nama) != 0)
      continue;
    if (modul_sama_ins(panggil, tabel_simbol[i].modul))
      return d;
  }

  for (int i = 0; i < jumlah_simbol; i++) {
    const InstruksiRA *d = tabel_simbol[i].deklarasi;
    if (strcmp(d->nama, panggil->nama) != 0)
      continue;
    for (int m = 0; m < jumlah_modul_muat; m++) {
      if (strcmp(modul_muat[m].var, "_") == 0 &&
          tabel_simbol[i].modul &&
          strcmp(modul_muat[m].modul, tabel_simbol[i].modul) == 0 &&
          tabel_simbol[i].publik)
        return d;
    }
  }

  return NULL;
}

static void ganti_nama(InstruksiRA *ins, const char *nama_baru) {
  free(ins->nama);
  ins->nama = salin(nama_baru, (int)strlen(nama_baru));
}

static void mangle(const InstruksiRA *daftar) {
  for (const InstruksiRA *cur = daftar; cur; cur = cur->next) {
    if (cur->badan)
      mangle(cur->badan);
  }

  for (InstruksiRA *cur = (InstruksiRA *)daftar; cur; cur = cur->next) {
    if (cur->eks)
      continue;

    const char *dot = strchr(cur->nama, '.');
    if (dot) {
      size_t len_var = (size_t)(dot - cur->nama);
      char *var = malloc(len_var + 1);
      memcpy(var, cur->nama, len_var);
      var[len_var] = '\0';
      const char *asli = modul_dari_var(var);
      free(var);
      free(cur->nama);
      cur->nama = salin(dot + 1, (int)strlen(dot + 1));
      free(cur->modul);
      cur->modul = asli ? salin_modul(asli) : NULL;
    }

    if (cur->tipe == RA_FUNGSI) {
      if (!cur->modul)
        continue;

      size_t n = strlen(cur->modul) + strlen(cur->nama) + 2;
      char *r = malloc(n);
      snprintf(r, n, "%s_%s", cur->modul, cur->nama);
      ganti_nama(cur, r);
      free(r);
      continue;
    }

    const InstruksiRA *d = cari_simbol(cur);
    if (!d)
      continue;

    free(cur->modul);
    cur->modul = salin_modul(d->modul);
    cur->eks = d->eks;
    cur->pub = d->pub;

    if (d->eks || !d->modul) {
      ganti_nama(cur, d->nama);
      continue;
    }

    size_t n = strlen(d->modul) + strlen(d->nama) + 2;
    char *r = malloc(n);
    snprintf(r, n, "%s_%s", d->modul, d->nama);
    ganti_nama(cur, r);
    free(r);
  }
}

static void ambil_anak(const PSA *induk, InstruksiRA **deklarasi,
                       InstruksiRA **ekor_deklarasi, InstruksiRA **badan,
                       InstruksiRA **ekor_badan) {
  for (int j = 0; j < induk->jumlah_anak; j++) {
    const PSA *item = induk->anak[j];

    if (item->tipe == PSA_DEKLARASI) {
      InstruksiRA *ins = calloc(1, sizeof(InstruksiRA));
      isi_dari_deklarasi(ins, item);
      ins->modul = salin_modul(item->modul);

      InstruksiRA *ekor_lokal = NULL;
      ambil_anak(item, NULL, NULL, &ins->badan, &ekor_lokal);
      if (deklarasi)
        dorong(deklarasi, ekor_deklarasi, ins);
      else
        dorong(badan, ekor_badan, ins);
      continue;
    }

    if (item->tipe == PSA_PANGGILAN) {
      InstruksiRA *ins = calloc(1, sizeof(InstruksiRA));
      isi_dari_panggilan(ins, item);
      ins->modul = salin_modul(item->modul);
      dorong(badan, ekor_badan, ins);
    }
  }
}

InstruksiRA *bangkitkan_ra(const PSA *akar) {
  InstruksiRA *kepala = NULL;
  InstruksiRA *ekor = NULL;
  InstruksiRA *utama = NULL;
  InstruksiRA *badan_utama = NULL;
  InstruksiRA *ekor_badan_utama = NULL;

  for (int i = 0; i < akar->jumlah_anak; i++) {
    const PSA *berkas = akar->anak[i];
    if (berkas->tipe != PSA_BERKAS)
      continue;

    InstruksiRA *titik = buat_titik_masuk();
    titik->modul = berkas->titik_masuk ? NULL : salin_modul(berkas->modul);
    titik->pub = !berkas->titik_masuk;

    InstruksiRA *badan_titik = NULL;
    InstruksiRA *ekor_badan_titik = NULL;

    for (int j = 0; j < berkas->jumlah_anak; j++) {
      const PSA *item = berkas->anak[j];

      if (item->tipe == PSA_DEKLARASI) {
        InstruksiRA *ins = calloc(1, sizeof(InstruksiRA));
        isi_dari_deklarasi(ins, item);
        ins->modul = salin_modul(berkas->modul);
        InstruksiRA *ekor_lokal = NULL;
        ambil_anak(item, NULL, NULL, &ins->badan, &ekor_lokal);
        dorong(&kepala, &ekor, ins);
      } else if (item->tipe == PSA_PANGGILAN) {
        InstruksiRA *ins = calloc(1, sizeof(InstruksiRA));
        isi_dari_panggilan(ins, item);
        ins->modul = salin_modul(berkas->modul);
        dorong(&badan_titik, &ekor_badan_titik, ins);
      }
    }

    titik->badan = badan_titik;

    if (berkas->titik_masuk) {
      for (InstruksiRA *b = badan_titik; b; b = b->next) {
        if (b->tipe != RA_PANGGIL)
          continue;
        InstruksiRA *k = calloc(1, sizeof(InstruksiRA));
        *k = *b;
        k->nama = salin(b->nama, (int)strlen(b->nama));
        k->modul = salin_modul(b->modul);
        k->badan = NULL;
        k->nilai = NULL;
        k->tipe_nilai = NULL;
        k->next = NULL;
        for (int q = 0; q < b->jumlah; q++) {
          k->nilai = realloc(k->nilai, sizeof(char *) * (q + 1));
          k->tipe_nilai = realloc(k->tipe_nilai, sizeof(TipeNilaiRA) * (q + 1));
          k->nilai[q] = salin(b->nilai[q], (int)strlen(b->nilai[q]));
          k->tipe_nilai[q] = b->tipe_nilai[q];
        }

        dorong(&badan_utama, &ekor_badan_utama, k);
      }

      utama = titik;
    } else {
      dorong(&kepala, &ekor, titik);

      InstruksiRA *panggil = calloc(1, sizeof(InstruksiRA));
      panggil->tipe = RA_PANGGIL;
      panggil->nama = salin("titik_masuk", 11);
      panggil->eks = false;
      panggil->pub = true;
      panggil->modul = salin_modul(berkas->modul);
      dorong(&badan_utama, &ekor_badan_utama, panggil);
    }
  }

  if (utama) {
    utama->badan = badan_utama;
    dorong(&kepala, &ekor, utama);
  }

  kumpul_modul_muat(akar);
  kumpulkan_simbol(kepala);
  mangle(kepala);

  free(tabel_simbol);
  tabel_simbol = NULL;
  jumlah_simbol = 0;

  for (int i = 0; i < jumlah_modul_muat; i++) {
    free(modul_muat[i].var);
    free(modul_muat[i].modul);
  }
  free(modul_muat);
  modul_muat = NULL;
  jumlah_modul_muat = 0;

  return kepala;
}

static const char *nama_RA(const InstruksiRA *ins) {
  return ins->tipe == RA_FUNGSI ? "fungsi" : "panggil";
}

static const char *nama_tipe_RA(TipeNilaiRA tipe) {
  switch (tipe) {
  case RA_BILANGAN:
    return "b32";
  case RA_UNTAIAN:
    return "unt";
  default:
    return "?";
  }
}

static void cetak_satu_ra(const InstruksiRA *ins, const char *prefix) {
  printf("%s%s %s(", prefix, nama_RA(ins), ins->nama);

  for (int i = 0; i < ins->jumlah; i++) {
    if (i > 0)
      printf(", ");

    if (ins->tipe_nilai[i] == RA_TANPA_TIPE) {
      printf("%s", ins->nilai[i]);
    } else {
      printf("%s %s", nama_tipe_RA(ins->tipe_nilai[i]), ins->nilai[i]);
    }
  }

  if (ins->tipe == RA_FUNGSI) {
    if (ins->eks)
      printf(") eks");
    else if (ins->pub)
      printf(") pub");
    else
      printf(")");
  } else
    printf(")");

  if (ins->badan) {
    printf(" {\n");
    for (const InstruksiRA *b = ins->badan; b; b = b->next)
      cetak_satu_ra(b, "  ");
    printf("%s}", prefix);
  }

  printf("\n");
}

static bool modul_sama(const InstruksiRA *ins, const char *m) {
  if (!ins->modul && !m)
    return true;
  if (!ins->modul || !m)
    return false;
  return strcmp(ins->modul, m) == 0;
}

static void kumpul_dari_satu(const InstruksiRA *ins, const char ***dipakai,
                             int *jumlah) {
  if (ins->badan)
    for (const InstruksiRA *b = ins->badan; b; b = b->next)
      kumpul_dari_satu(b, dipakai, jumlah);

  if (ins->tipe != RA_PANGGIL)
    return;

  for (int i = 0; i < *jumlah; i++)
    if (strcmp((*dipakai)[i], ins->nama) == 0)
      return;

  *dipakai = realloc(*dipakai, sizeof(char *) * (*jumlah + 1));
  (*dipakai)[(*jumlah)++] = ins->nama;
}

static InstruksiRA *klon(const InstruksiRA *d, const char *modul) {
  InstruksiRA *k = calloc(1, sizeof(InstruksiRA));
  k->tipe = d->tipe;
  k->nama = salin(d->nama, (int)strlen(d->nama));
  k->eks = d->eks;
  k->pub = d->pub;
  k->modul = modul ? salin_modul(modul) : NULL;
  k->jumlah = d->jumlah;

  if (d->jumlah > 0) {
    k->nilai = malloc(sizeof(char *) * d->jumlah);
    k->tipe_nilai = malloc(sizeof(TipeNilaiRA) * d->jumlah);
    for (int i = 0; i < d->jumlah; i++) {
      k->nilai[i] = salin(d->nilai[i], (int)strlen(d->nilai[i]));
      k->tipe_nilai[i] = d->tipe_nilai[i];
    }
  }

  return k;
}

static bool dipakai_di_grup(const char *nama, const InstruksiRA *grup) {
  if (grup->tipe == RA_PANGGIL && strcmp(grup->nama, nama) == 0)
    return true;

  for (const InstruksiRA *b = grup->badan; b; b = b->next)
    if (dipakai_di_grup(nama, b))
      return true;

  return false;
}

InstruksiRA *tambah_impor_ra(InstruksiRA *daftar, const InstruksiRA *sumber) {
  const InstruksiRA *deklarasi[256];
  int jumlah_deklarasi = 0;

  for (const InstruksiRA *s = sumber; s && jumlah_deklarasi < 256; s = s->next)
    if (s->tipe == RA_FUNGSI && (!s->badan || s->pub))
      deklarasi[jumlah_deklarasi++] = s;

  InstruksiRA *kepala = NULL;
  InstruksiRA *ekor = NULL;
  InstruksiRA *awal = daftar;

  while (awal) {
    const char *m = awal->modul;

    InstruksiRA *grup[256];
    int n = 0;

    for (InstruksiRA *g = awal; g && n < 256 && modul_sama(g, m); g = g->next)
      grup[n++] = g;
    if (n == 0)
      break;

    InstruksiRA *p = grup[n - 1]->next;

    const char *sudah[256];
    int jumlah_sudah = 0;

    for (int i = 0; i < n; i++)
      if (grup[i]->tipe == RA_FUNGSI) {
        sudah[jumlah_sudah++] = grup[i]->nama;
      }

    for (int j = 0; j < jumlah_deklarasi; j++) {
      const InstruksiRA *d = deklarasi[j];
      if (modul_sama(d, m))
        continue;

      bool perlu = false;
      for (int i = 0; i < n && !perlu; i++)
        perlu = dipakai_di_grup(d->nama, grup[i]);
      if (!perlu)
        continue;

      bool bentrok = false;
      for (int i = 0; i < n && !bentrok; i++)
        if (grup[i]->tipe == RA_FUNGSI && strcmp(grup[i]->nama, d->nama) == 0)
          bentrok = true;

      if (!bentrok)
        dorong(&kepala, &ekor, klon(d, m));
    }

    for (int i = 0; i < n; i++)
      dorong(&kepala, &ekor, grup[i]);

    awal = p;
  }

  return kepala;
}

void cetak_ra_permodul(const InstruksiRA *daftar) {
  const char *modul_sekarang = NULL;

  for (const InstruksiRA *ins = daftar; ins; ins = ins->next) {
    const char *m = ins->modul ? ins->modul : "<program>";

    if (!modul_sekarang || strcmp(m, modul_sekarang) != 0) {
      if (modul_sekarang)
        printf("selesai\n\n");
      printf("Berkas %s:\n", m);
      modul_sekarang = m;
    }

    cetak_satu_ra(ins, "");
  }

  if (modul_sekarang)
    printf("selesai\n");
}

void bersihkan_ra(InstruksiRA *daftar) {
  while (daftar) {
    InstruksiRA *next = daftar->next;
    for (int i = 0; i < daftar->jumlah; i++)
      free(daftar->nilai[i]);
    free(daftar->nilai);
    free(daftar->tipe_nilai);
    free(daftar->nama);
    free(daftar->modul);
    if (daftar->badan)
      bersihkan_ra(daftar->badan);
    free(daftar);
    daftar = next;
  }
}