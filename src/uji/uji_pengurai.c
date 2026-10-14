#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include "nusa/pengurai.h"

int main(void) {
  {
    const char *kode = "mod std mtk\nstd.cetak(\"halo\")\n";
    PSA *psa = urai(kode, "uji1.ns");
    assert(psa != NULL);
    assert(psa->tipe == PSA_PROGRAM);
    assert(psa->jumlah_anak == 1);
    PSA *berkas = psa->anak[0];
    assert(berkas->tipe == PSA_BERKAS);
    assert(berkas->titik_masuk == true);
    assert(berkas->jumlah_anak >= 2);

    PSA *mod_node = berkas->anak[0];
    assert(mod_node->tipe == PSA_MOD);
    assert(mod_node->jumlah_anak == 2);
    assert(mod_node->anak[0]->tipe == PSA_PENGENAL);
    assert(strcmp(mod_node->anak[0]->teks, "std") == 0);
    assert(mod_node->anak[1]->tipe == PSA_PENGENAL);
    assert(strcmp(mod_node->anak[1]->teks, "mtk") == 0);

    PSA *panggil = berkas->anak[1];
    assert(panggil->tipe == PSA_PANGGILAN);
    assert(strcmp(panggil->teks, "std.cetak") == 0);

    bersihkan_psa(psa);
  }

  {
    const char *kode = "mod\nstd\nmtk\n";
    PSA *psa = urai(kode, "uji2.ns");
    assert(psa != NULL);
    PSA *berkas = psa->anak[0];
    assert(berkas->jumlah_anak >= 1);
    PSA *mod_node = berkas->anak[0];
    assert(mod_node->tipe == PSA_MOD);
    assert(mod_node->jumlah_anak == 2);
    assert(strcmp(mod_node->anak[0]->teks, "std") == 0);
    assert(strcmp(mod_node->anak[1]->teks, "mtk") == 0);
    bersihkan_psa(psa);
  }

  {
    const char *kode = "mod mtk.op\nmtk.op.hitung(10)\n";
    PSA *psa = urai(kode, "uji3.ns");
    assert(psa != NULL);
    PSA *berkas = psa->anak[0];
    PSA *mod_node = berkas->anak[0];
    assert(mod_node->tipe == PSA_MOD);
    assert(mod_node->jumlah_anak == 1);
    assert(strcmp(mod_node->anak[0]->teks, "mtk.op") == 0);

    PSA *panggil = berkas->anak[1];
    assert(panggil->tipe == PSA_PANGGILAN);
    assert(strcmp(panggil->teks, "mtk.op.hitung") == 0);

    bersihkan_psa(psa);
  }

  {
    const char *kode = "mod std mtk.op geometri.lingkaran\n";
    PSA *psa = urai(kode, "uji4.ns");
    assert(psa != NULL);
    PSA *berkas = psa->anak[0];
    PSA *mod_node = berkas->anak[0];
    assert(mod_node->tipe == PSA_MOD);
    assert(mod_node->jumlah_anak == 3);
    assert(strcmp(mod_node->anak[0]->teks, "std") == 0);
    assert(strcmp(mod_node->anak[1]->teks, "mtk.op") == 0);
    assert(strcmp(mod_node->anak[2]->teks, "geometri.lingkaran") == 0);
    bersihkan_psa(psa);
  }

  {
    const char *kode = "# Komentar 1\n# Komentar 2\nmod std\n";
    PSA *psa = urai(kode, "uji5.ns");
    assert(psa != NULL);
    PSA *berkas = psa->anak[0];
    PSA *mod_node = berkas->anak[0];
    assert(mod_node->tipe == PSA_MOD);
    assert(mod_node->jumlah_anak == 1);
    assert(strcmp(mod_node->anak[0]->teks, "std") == 0);
    bersihkan_psa(psa);
  }

  {
    const char *kode = "mod std\nmod mtk\n";
    PSA *psa = urai(kode, "uji_ganda.ns");
    assert(psa == NULL);
  }

  {
    const char *kode = "utama() {}\nmod std\n";
    PSA *psa = urai(kode, "uji_posisi.ns");
    assert(psa == NULL);
  }

  {
    const char *kode = "cetak(teks unt) eks\nmod std\n";
    PSA *psa = urai(kode, "uji_posisi2.ns");
    assert(psa == NULL);
  }

  {
    const char *daftar[] = {"contoh/halodunia.ns", "kode/std.ns"};
    PSA *psa = urai_banyak_berkas(daftar, 2);
    assert(psa != NULL);
    assert(psa->tipe == PSA_PROGRAM);
    assert(psa->jumlah_anak == 2);
    assert(psa->anak[0]->titik_masuk == true);
    assert(psa->anak[1]->titik_masuk == false);
    assert(strcmp(psa->anak[0]->jalur, "contoh/halodunia.ns") == 0);
    assert(strcmp(psa->anak[1]->jalur, "kode/std.ns") == 0);
    bersihkan_psa(psa);
  }

  return 0;
}

