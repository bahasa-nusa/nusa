// Copyright 2026 Pengembang Bahasa Pemrograman Nusa
// SPDX-License-Identifier: Apache-2.0

#include "nusa/berkas.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#include <windows.h>

#define F_OK 0
#define access _access
#define mkdir _mkdir
#else
#include <libgen.h>
#include <limits.h>
#include <unistd.h>

#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

typedef struct EntriDimuat {
  char *jalur;
  struct EntriDimuat *lanjut;
} EntriDimuat;

static EntriDimuat *daftar_dimuat = NULL;

/* Dua akar pencarian muat berkas */
char *direktori_berkas_utama = NULL;
char *direktori_instalasi = NULL;

static char *salin_string(const char *s) {
  if (!s)
    return NULL;

  size_t p = strlen(s) + 1;
  char *r = malloc(p);
  if (!r)
    return NULL;

  memcpy(r, s, p);
  return r;
}

#ifdef _WIN32
static char *jalur_absolut_win(const char *jalur) {
  char buf[MAX_PATH];

  DWORD n = GetFullPathNameA(jalur, MAX_PATH, buf, NULL);
  if (n == 0 || n >= MAX_PATH)
    return salin_string(jalur);

  return salin_string(buf);
}
#endif

char *jalur_kanonis(const char *jalur) {
  if (!jalur)
    return NULL;
#ifdef _WIN32
  char *a = jalur_absolut_win(jalur);
  if (!a)
    return NULL;

  for (char *c = a; *c; c++) {
    if (*c == '\\')
      *c = '/';
  }

  return a;
#else
  char buf[PATH_MAX];

  if (realpath(jalur, buf)) {
    return salin_string(buf);
  }

  return salin_string(jalur);
#endif
}

char *direktori_dari(const char *jalur) {
  if (!jalur)
    return NULL;

  char *k = jalur_kanonis(jalur);
  if (!k)
    return NULL;

  char *slash = strrchr(k, '/');
  if (!slash) {
    free(k);
    return NULL;
  }

  if (slash == k) {
    slash[1] = '\0';
    return k;
  }

  *slash = '\0';
  return k;
}

char *jalur_biner(void) {
#ifdef _WIN32
  char buf[MAX_PATH];

  DWORD n = GetModuleFileNameA(NULL, buf, MAX_PATH);
  if (n == 0 || n >= MAX_PATH)
    return NULL;

  return salin_string(buf);
#else
  char buf[PATH_MAX];

  ssize_t n = readlink("/proc/self/exe", buf, PATH_MAX - 1);
  if (n <= 0)
    return NULL;

  buf[n] = '\0';
  return salin_string(buf);
#endif
}

static bool berkas_ada(const char *jalur) {
  struct stat st;
  return jalur && stat(jalur, &st) == 0 && S_ISREG(st.st_mode);
}

static char *gabung_akar(const char *akar, const char *nama) {
  if (!akar || !nama)
    return NULL;

  size_t la = strlen(akar);
  size_t ln = strlen(nama);
  bool perlu_pemisah = la > 0 && akar[la - 1] != '/' && akar[la - 1] != '\\';
  size_t total = la + (perlu_pemisah ? 1 : 0) + ln + 1;

  char *r = malloc(total);
  if (!r)
    return NULL;

  snprintf(r, total, perlu_pemisah ? "%s/%s" : "%s%s", akar, nama);
  return r;
}

char *cari_berkas(const char *nama) {
  if (!nama || nama[0] == '\0')
    return NULL;

#ifdef _WIN32
  bool absolut = nama[1] == ':' || nama[0] == '/' || nama[0] == '\\';
#else
  bool absolut = nama[0] == '/';
#endif
  if (absolut)
    return berkas_ada(nama) ? jalur_kanonis(nama) : NULL;

  char *kandidat = gabung_akar(direktori_berkas_utama, nama);
  if (kandidat) {
    if (berkas_ada(kandidat)) {
      char *k = jalur_kanonis(kandidat);
      free(kandidat);
      return k;
    }
    free(kandidat);
  }

  char *akar_kode = gabung_akar(direktori_instalasi, "kode");
  kandidat = gabung_akar(akar_kode, nama);
  free(akar_kode);
  if (kandidat) {
    if (berkas_ada(kandidat)) {
      char *k = jalur_kanonis(kandidat);
      free(kandidat);
      return k;
    }
    free(kandidat);
  }

  return NULL;
}

const char *baca_berkas(const char *nama_berkas) {
  FILE *berkas = fopen(nama_berkas, "rb");
  if (!berkas) {
    return NULL;
  }

  if (fseek(berkas, 0, SEEK_END) != 0) {
    fclose(berkas);
    return NULL;
  }

  long ukuran = ftell(berkas);

  if (ukuran < 0) {
    fclose(berkas);
    return NULL;
  }

  rewind(berkas);

  char *isi = (char *)malloc((size_t)ukuran + 1);
  if (!isi) {
    fclose(berkas);
    return NULL;
  }

  size_t terbaca = ukuran ? fread(isi, 1, (size_t)ukuran, berkas) : 0;

  fclose(berkas);

  isi[terbaca] = '\0';
  return isi;
}

void bersihkan_berkas(const char *isi_berkas) { free((void *)isi_berkas); }

bool berkas_sudah_dimuat(const char *jalur_resolusi) {
  if (!jalur_resolusi)
    return false;

  char *k = jalur_kanonis(jalur_resolusi);
  if (!k)
    return false;

  for (EntriDimuat *e = daftar_dimuat; e; e = e->lanjut) {
    if (e->jalur && strcmp(e->jalur, k) == 0) {
      free(k);
      return true;
    }
  }

  free(k);
  return false;
}

void tandai_berkas_dimuat(const char *jalur_resolusi) {
  if (!jalur_resolusi)
    return;

  char *k = jalur_kanonis(jalur_resolusi);
  if (!k)
    return;

  if (berkas_sudah_dimuat(k)) {
    free(k);
    return;
  }

  EntriDimuat *e = malloc(sizeof(EntriDimuat));
  if (!e) {
    free(k);
    return;
  }

  e->jalur = k;
  e->lanjut = daftar_dimuat;
  daftar_dimuat = e;
}

void bersihkan_daftar_dimuat(void) {
  EntriDimuat *e = daftar_dimuat;

  while (e) {
    EntriDimuat *n = e->lanjut;

    free(e->jalur);
    free(e);

    e = n;
  }

  daftar_dimuat = NULL;
}

char *gabung_jalur_relatif(const char *dasar, const char *jalur) {
  if (!jalur)
    return NULL;

#ifdef _WIN32
  if (jalur[0] != '\0' &&
      (jalur[1] == ':' || jalur[0] == '/' || jalur[0] == '\\')) {
    return jalur_kanonis(jalur);
  }
#else
  if (jalur[0] == '/') {
    return jalur_kanonis(jalur);
  }
#endif
  if (!dasar || dasar[0] == '\0') {
    return jalur_kanonis(jalur);
  }

  char *dasar_copy = salin_string(dasar);
  if (!dasar_copy)
    return jalur_kanonis(jalur);
#ifdef _WIN32
  char drive[_MAX_DRIVE];
  char dir[_MAX_DIR];
  char fname[_MAX_FNAME];
  char ext[_MAX_EXT];
  _splitpath_s(dasar_copy, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME,
               ext, _MAX_EXT);

  char gabung[MAX_PATH];
  _makepath_s(gabung, MAX_PATH, drive, dir, "", "");

  char *dir_dasar = gabung;
  size_t len = strlen(dir_dasar) + strlen(jalur) + 2;

  char *tmp = malloc(len);
  if (!tmp) {
    free(dasar_copy);
    return jalur_kanonis(jalur);
  }

  snprintf(tmp, len, "%s%s", dir_dasar, jalur);

  free(dasar_copy);

  char *res = jalur_kanonis(tmp);

  free(tmp);
  return res;
#else
  char *dasar_dir = dirname(dasar_copy);
  size_t len = strlen(dasar_dir) + 1 + strlen(jalur) + 1;

  char *tmp = malloc(len);
  if (!tmp) {
    free(dasar_copy);
    return jalur_kanonis(jalur);
  }

  snprintf(tmp, len, "%s/%s", dasar_dir, jalur);

  free(dasar_copy);

  char *res = jalur_kanonis(tmp);

  free(tmp);
  return res;
#endif
}