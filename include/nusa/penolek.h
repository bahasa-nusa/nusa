#ifndef NUSA_PENOLEK_H
#define NUSA_PENOLEK_H

typedef enum {
  TIPE_TOLEK_AKHIR,
  TIPE_TOLEK_TIDAK_DIKETAHUI,
  TIPE_TOLEK_KOMENTAR,
  TIPE_TOLEK_NILAI_UNTAIAN,
  TIPE_TOLEK_NILAI_BILANGAN,
  TIPE_TOLEK_PENGENAL,
  TIPE_TOLEK_KURUNG_BULAT_BUKA,
  TIPE_TOLEK_KURUNG_BULAT_TUTUP,
  TIPE_TOLEK_KATA_KUNCI_EKSTERNAL, // eks
  TIPE_TOLEK_KATA_KUNCI_PUBLIK,    // pub
  TIPE_TOLEK_TIPE_DATA_BILANGAN,   // b32
  TIPE_TOLEK_TIPE_DATA_UNTAIAN,    // unt
} TipeTolek;

typedef struct {
  TipeTolek tipe;
  const char *teks;
  int panjang;
} Tolek;

const char *penolek(const char *isi, Tolek *hasil);
const char *nama_tolek(TipeTolek tipe);

#endif
