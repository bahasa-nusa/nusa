#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include "nusa/pengurai.h"
#include "nusa/pesemantik.h"

int main(void) {
  {
    PSA *akar = urai("mod std\nutama() {\nstd.cetak(\"halo\")\n}\n", "program.ns");
    assert(akar != NULL);
    PSA *std_mod = urai("cetak(teks unt) pub {\n}\n", "std.ns");
    assert(std_mod != NULL);

    akar->jumlah_anak++;
    akar->anak = realloc(akar->anak, sizeof(PSA *) * 2);
    akar->anak[1] = std_mod->anak[0];
    std_mod->anak[0] = NULL;
    std_mod->jumlah_anak = 0;
    bersihkan_psa(std_mod);

    int galat = pesemantik(akar);
    assert(galat == 0);
    bersihkan_psa(akar);
  }

  {
    PSA *akar = urai("mod std\nutama() {\nstd.cetak(\"halo\")\n}\n", "program.ns");
    assert(akar != NULL);
    int galat = pesemantik(akar);
    assert(galat > 0);
    bersihkan_psa(akar);
  }

  {
    const char *daftar_nama[] = {"mod_b.ns", "mod_a.ns"};
    PSA *akar = urai("mod mod_a\nutama() {\nmod_a.rahasia()\n}\n", daftar_nama[0]);
    assert(akar != NULL);
    PSA *mod_a = urai("rahasia() {\n}\n", daftar_nama[1]);
    assert(mod_a != NULL);

    PSA *program = urai_banyak_berkas(NULL, 0);
    assert(program == NULL);

    akar->jumlah_anak++;
    akar->anak = realloc(akar->anak, sizeof(PSA *) * 2);
    akar->anak[1] = mod_a->anak[0];
    mod_a->anak[0] = NULL;
    mod_a->jumlah_anak = 0;
    bersihkan_psa(mod_a);

    akar->anak[0]->titik_masuk = true;
    akar->anak[1]->titik_masuk = false;

    int galat = pesemantik(akar);
    assert(galat > 0);
    bersihkan_psa(akar);
  }

  {
    PSA *akar = urai("mod mod_a\nutama() {\nmod_a.terbuka()\n}\n", "mod_b.ns");
    assert(akar != NULL);
    PSA *mod_a = urai("terbuka() pub {\n}\n", "mod_a.ns");
    assert(mod_a != NULL);

    akar->jumlah_anak++;
    akar->anak = realloc(akar->anak, sizeof(PSA *) * 2);
    akar->anak[1] = mod_a->anak[0];
    mod_a->anak[0] = NULL;
    mod_a->jumlah_anak = 0;
    bersihkan_psa(mod_a);

    akar->anak[0]->titik_masuk = true;
    akar->anak[1]->titik_masuk = false;

    int galat = pesemantik(akar);
    assert(galat == 0);
    bersihkan_psa(akar);
  }

  {
    PSA *akar = urai("mod mtk.op\nutama() {\nmtk.op.hitung(10)\n}\n", "utama.ns");
    assert(akar != NULL);
    PSA *mod_mtk = urai("hitung(x b32) pub {\n}\n", "mtk/op.ns");
    assert(mod_mtk != NULL);

    akar->jumlah_anak++;
    akar->anak = realloc(akar->anak, sizeof(PSA *) * 2);
    akar->anak[1] = mod_mtk->anak[0];
    mod_mtk->anak[0] = NULL;
    mod_mtk->jumlah_anak = 0;
    bersihkan_psa(mod_mtk);

    int galat = pesemantik(akar);
    assert(galat == 0);
    bersihkan_psa(akar);
  }

  {
    PSA *akar = urai("utama() {\nmod_a.terbuka()\n}\n", "mod_b.ns");
    assert(akar != NULL);
    PSA *mod_a = urai("terbuka() pub {\n}\n", "mod_a.ns");
    assert(mod_a != NULL);

    akar->jumlah_anak++;
    akar->anak = realloc(akar->anak, sizeof(PSA *) * 2);
    akar->anak[1] = mod_a->anak[0];
    mod_a->anak[0] = NULL;
    mod_a->jumlah_anak = 0;
    bersihkan_psa(mod_a);

    int galat = pesemantik(akar);
    assert(galat > 0);
    bersihkan_psa(akar);
  }

  {
    PSA *akar = urai("mod mod_a\nutama() {\nmod_a.tidak_ada()\n}\n", "mod_b.ns");
    assert(akar != NULL);
    PSA *mod_a = urai("ada() pub {\n}\n", "mod_a.ns");
    assert(mod_a != NULL);

    akar->jumlah_anak++;
    akar->anak = realloc(akar->anak, sizeof(PSA *) * 2);
    akar->anak[1] = mod_a->anak[0];
    mod_a->anak[0] = NULL;
    mod_a->jumlah_anak = 0;
    bersihkan_psa(mod_a);

    int galat = pesemantik(akar);
    assert(galat > 0);
    bersihkan_psa(akar);
  }

  {
    PSA *akar = urai("mod mod_a\nutama() {\nmod_a.tambah(1)\n}\n", "mod_b.ns");
    assert(akar != NULL);
    PSA *mod_a = urai("tambah(a b32, b b32) pub {\n}\n", "mod_a.ns");
    assert(mod_a != NULL);

    akar->jumlah_anak++;
    akar->anak = realloc(akar->anak, sizeof(PSA *) * 2);
    akar->anak[1] = mod_a->anak[0];
    mod_a->anak[0] = NULL;
    mod_a->jumlah_anak = 0;
    bersihkan_psa(mod_a);

    int galat = pesemantik(akar);
    assert(galat > 0);
    bersihkan_psa(akar);
  }

  {
    PSA *akar = urai("mod mod_a\nutama() {\nmod_a.cetak_angka(\"salah\")\n}\n", "mod_b.ns");
    assert(akar != NULL);
    PSA *mod_a = urai("cetak_angka(x b32) pub {\n}\n", "mod_a.ns");
    assert(mod_a != NULL);

    akar->jumlah_anak++;
    akar->anak = realloc(akar->anak, sizeof(PSA *) * 2);
    akar->anak[1] = mod_a->anak[0];
    mod_a->anak[0] = NULL;
    mod_a->jumlah_anak = 0;
    bersihkan_psa(mod_a);

    int galat = pesemantik(akar);
    assert(galat > 0);
    bersihkan_psa(akar);
  }

  {
    PSA *akar = urai("tes(x b32) pub {\n}\ntes(y b32) pub {\n}\n", "mod_a.ns");
    assert(akar != NULL);
    int galat = pesemantik(akar);
    assert(galat > 0);
    bersihkan_psa(akar);
  }

  {
    PSA *akar = urai("rahasia() {\n}\nutama() {\nrahasia()\n}\n", "lokal.ns");
    assert(akar != NULL);
    int galat = pesemantik(akar);
    assert(galat == 0);
    bersihkan_psa(akar);
  }

  return 0;
}
