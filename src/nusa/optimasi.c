#include "nusa/optimasi.h"
#include "nusa/ra.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  const char *nama;
  const InstruksiRA *fungsi;
  bool hidup;
} SimbolRA;

static SimbolRA *tabel;
static int jumlah_tabel;

static void dorong(InstruksiRA **kepala, InstruksiRA **ekor, InstruksiRA *ins) {
  ins->next = NULL;
  if (!*kepala) {
    *kepala = *ekor = ins;
    return;
  }
  (*ekor)->next = ins;
  *ekor = ins;
}

static void kumpulkan(const InstruksiRA *daftar) {
  for (const InstruksiRA *cur = daftar; cur; cur = cur->next) {
    if (cur->tipe == RA_FUNGSI) {
      tabel = realloc(tabel, sizeof(SimbolRA) * (jumlah_tabel + 1));
      tabel[jumlah_tabel].nama = cur->nama;
      tabel[jumlah_tabel].fungsi = cur;
      tabel[jumlah_tabel].hidup = false;
      jumlah_tabel++;
    }

    if (cur->badan)
      kumpulkan(cur->badan);
  }
}

static int cari(const char *nama) {
  for (int i = 0; i < jumlah_tabel; i++)
    if (strcmp(tabel[i].nama, nama) == 0)
      return i;

  return -1;
}

static void tandai(const InstruksiRA *daftar) {
  for (const InstruksiRA *cur = daftar; cur; cur = cur->next) {
    if (cur->badan)
      tandai(cur->badan);
    if (cur->tipe != RA_PANGGIL)
      continue;

    int idx = cari(cur->nama);
    if (idx < 0 || tabel[idx].hidup)
      continue;

    const InstruksiRA *f = tabel[idx].fungsi;
    if (!f->eks && !f->badan)
      continue;

    tabel[idx].hidup = true;
    tandai(f->badan);
  }
}

static bool hidup(const char *nama) {
  int idx = cari(nama);
  return idx < 0 || tabel[idx].hidup;
}

static InstruksiRA *optimalkan_dengan(const InstruksiRA *daftar) {
  InstruksiRA *kepala = NULL;
  InstruksiRA *ekor = NULL;

  for (const InstruksiRA *cur = daftar; cur; cur = cur->next) {
    if (cur->tipe == RA_FUNGSI && !cur->eks && !cur->badan)
      continue;
    if (cur->tipe == RA_PANGGIL && !hidup(cur->nama))
      continue;

    InstruksiRA *kopi = calloc(1, sizeof(InstruksiRA));
    kopi->tipe = cur->tipe;
    kopi->nama = strdup(cur->nama);
    kopi->jumlah = cur->jumlah;
    kopi->eks = cur->eks;
    kopi->pub = cur->pub;
    kopi->modul = cur->modul ? strdup(cur->modul) : NULL;
    kopi->badan = cur->badan ? optimalkan_dengan(cur->badan) : NULL;

    if (cur->jumlah > 0) {
      kopi->nilai = malloc(sizeof(char *) * cur->jumlah);
      kopi->tipe_nilai = malloc(sizeof(TipeNilaiRA) * cur->jumlah);
      for (int i = 0; i < cur->jumlah; i++) {
        kopi->nilai[i] = strdup(cur->nilai[i]);
        kopi->tipe_nilai[i] = cur->tipe_nilai[i];
      }
    }

    dorong(&kepala, &ekor, kopi);
  }

  return kepala;
}

static bool modul_sama(const InstruksiRA *a, const InstruksiRA *b) {
  if (!a->modul && !b->modul)
    return true;
  if (!a->modul || !b->modul)
    return false;
  return strcmp(a->modul, b->modul) == 0;
}

static InstruksiRA *buang_modul_kosong(InstruksiRA *daftar) {
  InstruksiRA *kepala = NULL;
  InstruksiRA *ekor = NULL;

  while (daftar) {
    InstruksiRA *grup[256];
    int n = 0;

    for (InstruksiRA *g = daftar; g && n < 256 && modul_sama(g, daftar);
         g = g->next)
      grup[n++] = g;
    if (n == 0)
      break;

    InstruksiRA *p = grup[n - 1]->next;

    bool ada_kode = false;
    for (int i = 0; i < n; i++)
      if (grup[i]->badan)
        ada_kode = true;

    if (ada_kode) {
      for (int i = 0; i < n; i++)
        dorong(&kepala, &ekor, grup[i]);
    }

    daftar = p;
  }

  return kepala;
}

InstruksiRA *optimalkan(const InstruksiRA *daftar) {
  kumpulkan(daftar);

  for (int i = 0; i < jumlah_tabel; i++)
    if (tabel[i].fungsi->eks)
      tabel[i].hidup = true;

  for (const InstruksiRA *cur = daftar; cur; cur = cur->next) {
    if (cur->tipe != RA_FUNGSI || cur->modul)
      continue;
    if (strcmp(cur->nama, "titik_masuk") != 0)
      continue;

    int idx = cari(cur->nama);
    if (idx < 0)
      continue;

    tabel[idx].hidup = true;
    tandai(cur->badan);
  }

  InstruksiRA *hasil = buang_modul_kosong(optimalkan_dengan(daftar));

  free(tabel);
  tabel = NULL;
  jumlah_tabel = 0;

  return hasil;
}