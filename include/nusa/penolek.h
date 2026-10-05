#ifndef NUSA_PENOLEK_H
#define NUSA_PENOLEK_H

typedef enum {
  TIPE_TOLEK_AKHIR,
  TIPE_TOLEK_TIDAK_DIKETAHUI,
  TIPE_TOLEK_KOMENTAR,
  TIPE_TOLEK_UNTAIAN,
  TIPE_TOLEK_PENGENAL,
  TIPE_TOLEK_KURUNG_BULAT_BUKA,
  TIPE_TOLEK_KURUNG_BULAT_TUTUP,
} TipeTolek;

typedef struct {
  TipeTolek tipe;
  const char *teks;
  int panjang;
} Tolek;

const char *penolek(const char *isi, Tolek *hasil);
const char *nama_tolek(TipeTolek tipe);

#endif
