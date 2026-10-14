#include <assert.h>
#include <string.h>
#include "nusa/peleksim.h"

int main(void) {
  {
    assert(strcmp(nama_leksim(TIPE_LEKSIM_KATA_KUNCI_MOD), "MOD") == 0);
  }

  {
    Leksim hasil;
    const char *sisa = peleksim("mod", &hasil);
    assert(hasil.tipe == TIPE_LEKSIM_KATA_KUNCI_MOD);
    assert(hasil.panjang == 3);
    assert(strncmp(hasil.teks, "mod", 3) == 0);

    sisa = peleksim(sisa, &hasil);
    assert(hasil.tipe == TIPE_LEKSIM_AKHIR);
  }

  {
    Leksim hasil;
    const char *sisa = peleksim("muat", &hasil);
    assert(hasil.tipe == TIPE_LEKSIM_PENGENAL);
    assert(hasil.panjang == 4);
    assert(strncmp(hasil.teks, "muat", 4) == 0);

    sisa = peleksim(sisa, &hasil);
    assert(hasil.tipe == TIPE_LEKSIM_AKHIR);
  }

  {
    Leksim hasil;
    const char *sisa = peleksim("modul", &hasil);
    assert(hasil.tipe == TIPE_LEKSIM_PENGENAL);
    assert(hasil.panjang == 5);
    assert(strncmp(hasil.teks, "modul", 5) == 0);

    sisa = peleksim(sisa, &hasil);
    assert(hasil.tipe == TIPE_LEKSIM_AKHIR);
  }

  {
    const char *kode = "mod std mtk";
    Leksim hasil;
    const char *sisa = peleksim(kode, &hasil);
    assert(hasil.tipe == TIPE_LEKSIM_KATA_KUNCI_MOD);
    assert(hasil.panjang == 3);

    sisa = peleksim(sisa, &hasil);
    assert(hasil.tipe == TIPE_LEKSIM_PENGENAL);
    assert(hasil.panjang == 3);
    assert(strncmp(hasil.teks, "std", 3) == 0);

    sisa = peleksim(sisa, &hasil);
    assert(hasil.tipe == TIPE_LEKSIM_PENGENAL);
    assert(hasil.panjang == 3);
    assert(strncmp(hasil.teks, "mtk", 3) == 0);

    sisa = peleksim(sisa, &hasil);
    assert(hasil.tipe == TIPE_LEKSIM_AKHIR);
  }

  {
    const char *kode = "mod\nstd\nmtk";
    Leksim hasil;
    const char *sisa = peleksim(kode, &hasil);
    assert(hasil.tipe == TIPE_LEKSIM_KATA_KUNCI_MOD);

    sisa = peleksim(sisa, &hasil);
    assert(hasil.tipe == TIPE_LEKSIM_PENGENAL);
    assert(strncmp(hasil.teks, "std", 3) == 0);

    sisa = peleksim(sisa, &hasil);
    assert(hasil.tipe == TIPE_LEKSIM_PENGENAL);
    assert(strncmp(hasil.teks, "mtk", 3) == 0);

    sisa = peleksim(sisa, &hasil);
    assert(hasil.tipe == TIPE_LEKSIM_AKHIR);
  }

  {
    const char *kode = "mod mtk.op";
    Leksim hasil;
    const char *sisa = peleksim(kode, &hasil);
    assert(hasil.tipe == TIPE_LEKSIM_KATA_KUNCI_MOD);

    sisa = peleksim(sisa, &hasil);
    assert(hasil.tipe == TIPE_LEKSIM_PENGENAL);
    assert(strncmp(hasil.teks, "mtk", 3) == 0);

    sisa = peleksim(sisa, &hasil);
    assert(hasil.tipe == TIPE_LEKSIM_TITIK);

    sisa = peleksim(sisa, &hasil);
    assert(hasil.tipe == TIPE_LEKSIM_PENGENAL);
    assert(strncmp(hasil.teks, "op", 2) == 0);

    sisa = peleksim(sisa, &hasil);
    assert(hasil.tipe == TIPE_LEKSIM_AKHIR);
  }

  return 0;
}

