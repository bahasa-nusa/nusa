#include "nusa/berkas.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <io.h>
#include <windows.h>

#define F_OK 0
#define access _access
#else
#include <libgen.h>
#include <limits.h>
#include <unistd.h>

#endif

typedef struct EntriDimuat {
  char *jalur;
  struct EntriDimuat *lanjut;
} EntriDimuat;

static EntriDimuat *daftar_dimuat = NULL;

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