#include "nusa/pengurai.h"
#include "nusa/berkas.h"
#include "nusa/peleksim.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *pesan_kesalahan = NULL;

const char *pesan_urai(void) { return pesan_kesalahan; }

static char *salin_n(const char *s, int n) {
  if (!s)
    return NULL;
  char *r = malloc((size_t)n + 1);
  if (!r)
    return NULL;
  memcpy(r, s, (size_t)n);
  r[n] = '\0';
  return r;
}

static char *salin(const char *s) {
  if (!s)
    return NULL;
  return salin_n(s, (int)strlen(s));
}

static PSA *buat_node(TipePSA tipe, const char *teks, int panjang) {
  PSA *node = malloc(sizeof(PSA));
  node->tipe = tipe;
  node->teks = salin_n(teks, panjang);
  node->panjang = panjang;
  node->jalur = NULL;
  node->modul = NULL;
  node->titik_masuk = false;
  node->anak = NULL;
  node->jumlah_anak = 0;

  return node;
}

static char *modul_dari_jalur(const char *jalur) {
  if (!jalur)
    return NULL;

  const char *mulai = jalur;
  if (strncmp(mulai, "./", 2) == 0 || strncmp(mulai, ".\\", 2) == 0)
    mulai += 2;

  size_t pjg = strlen(mulai);
  if (pjg >= 3 && strcmp(mulai + pjg - 3, ".ns") == 0)
    pjg -= 3;

  char *r = salin_n(mulai, (int)pjg);
  if (!r)
    return NULL;

  for (char *c = r; *c; c++) {
    if (*c == '/' || *c == '\\')
      *c = '.';
  }

  return r;
}

static void tambah_anak(PSA *induk, PSA *anak) {
  induk->jumlah_anak++;
  induk->anak = realloc(induk->anak, sizeof(PSA *) * (size_t)induk->jumlah_anak);
  induk->anak[induk->jumlah_anak - 1] = anak;
}

void bersihkan_psa(PSA *node) {
  if (!node)
    return;

  for (int i = 0; i < node->jumlah_anak; i++) {
    bersihkan_psa(node->anak[i]);
  }

  free(node->anak);
  free(node->teks);
  free(node->jalur);
  free(node->modul);
  free(node);
}

void cetak_psa(const PSA *node, int indent) {
  if (!node)
    return;

  if (node->tipe == PSA_PROGRAM) {
    bool pertama = true;
    for (int i = 0; i < node->jumlah_anak; i++) {
      const PSA *anak = node->anak[i];
      if (anak->tipe != PSA_BERKAS)
        continue;
      if (!pertama)
        printf("\n");
      pertama = false;
      printf("Berkas %d: %s%s\n", i + 1, anak->jalur,
             anak->titik_masuk ? " (titik masuk)" : "");
      for (int j = 0; j < anak->jumlah_anak; j++) {
        cetak_psa(anak->anak[j], 1);
      }
    }
    return;
  }

  for (int i = 0; i < indent; i++)
    printf("  ");
  static const char *nama_tipe[] = {"PROGRAM",
                                    "BERKAS",
                                    "PANGGILAN",
                                    "DEKLARASI",
                                    "PENGENAL",
                                    "NILAI UNTAIAN",
                                    "NILAI BILANGAN",
                                    "KATA KUNCI",
                                    "KATA KUNCI PUBLIK",
                                    "TIPE DATA B32",
                                    "TIPE DATA UNTAIAN",
                                    "MOD",
                                    "TITIK"};
  printf("[%s] %.*s\n", nama_tipe[node->tipe], node->panjang, node->teks);

  for (int i = 0; i < node->jumlah_anak; i++) {
    cetak_psa(node->anak[i], indent + 1);
  }
}

static bool deklarasi_menunggu(const char *ptr) {
  Leksim t;
  while ((ptr = peleksim(ptr, &t)) && t.tipe != TIPE_LEKSIM_AKHIR) {
    if (t.tipe == TIPE_LEKSIM_KURUNG_BULAT_TUTUP)
      break;
    if (t.tipe == TIPE_LEKSIM_TIPE_DATA_UNTAIAN ||
        t.tipe == TIPE_LEKSIM_TIPE_DATA_BILANGAN) {
      return true;
    }
  }
  return false;
}

static const char *urai_argumen(const char *ptr, PSA *node) {
  Leksim lanjut;
  while ((ptr = peleksim(ptr, &lanjut)) &&
         lanjut.tipe != TIPE_LEKSIM_KURUNG_BULAT_TUTUP &&
         lanjut.tipe != TIPE_LEKSIM_AKHIR) {
    if (lanjut.tipe == TIPE_LEKSIM_PENGENAL) {
      tambah_anak(node, buat_node(PSA_PENGENAL, lanjut.teks, lanjut.panjang));
    } else if (lanjut.tipe == TIPE_LEKSIM_NILAI_UNTAIAN) {
      tambah_anak(node, buat_node(PSA_NILAI_UNTAIAN, lanjut.teks, lanjut.panjang));
    } else if (lanjut.tipe == TIPE_LEKSIM_NILAI_BILANGAN) {
      tambah_anak(node, buat_node(PSA_NILAI_BILANGAN, lanjut.teks, lanjut.panjang));
    } else if (lanjut.tipe == TIPE_LEKSIM_TIPE_DATA_UNTAIAN) {
      tambah_anak(node, buat_node(PSA_TIPE_DATA_UNTAIAN, lanjut.teks, lanjut.panjang));
    } else if (lanjut.tipe == TIPE_LEKSIM_TIPE_DATA_BILANGAN) {
      tambah_anak(node, buat_node(PSA_TIPE_DATA_BILANGAN, lanjut.teks, lanjut.panjang));
    } else if (lanjut.tipe == TIPE_LEKSIM_KATA_KUNCI_EKSTERNAL) {
      tambah_anak(node, buat_node(PSA_KATA_KUNCI, lanjut.teks, lanjut.panjang));
    } else if (lanjut.tipe == TIPE_LEKSIM_KATA_KUNCI_PUBLIK) {
      tambah_anak(node, buat_node(PSA_KATA_KUNCI_PUBLIK, lanjut.teks, lanjut.panjang));
    }
  }
  return ptr;
}

static const char *leksim_berikut(const char *ptr, Leksim *hasil) {
  const char *p = ptr;
  while ((p = peleksim(p, hasil)) && hasil->tipe == TIPE_LEKSIM_KOMENTAR) {
  }
  return p;
}

static char *urai_pengenal_modul(const char **ptr_masuk) {
  Leksim t1;
  const char *p = leksim_berikut(*ptr_masuk, &t1);
  if (t1.tipe != TIPE_LEKSIM_PENGENAL)
    return NULL;

  Leksim t2;
  leksim_berikut(p, &t2);
  if (t2.tipe == TIPE_LEKSIM_KURUNG_BULAT_BUKA || t2.tipe == TIPE_LEKSIM_OPERASI_ISI)
    return NULL;

  if (t2.tipe != TIPE_LEKSIM_TITIK) {
    *ptr_masuk = p;
    return salin_n(t1.teks, t1.panjang);
  }

  size_t kapasitas = 128;
  char *hasil = malloc(kapasitas);
  if (!hasil)
    return NULL;

  size_t len = (size_t)t1.panjang;
  memcpy(hasil, t1.teks, len);
  hasil[len] = '\0';

  const char *pos = p;
  while (true) {
    Leksim titik;
    const char *pos_titik = leksim_berikut(pos, &titik);
    if (titik.tipe != TIPE_LEKSIM_TITIK)
      break;

    Leksim sub;
    const char *pos_sub = leksim_berikut(pos_titik, &sub);
    if (sub.tipe != TIPE_LEKSIM_PENGENAL) {
      free(hasil);
      return NULL;
    }

    Leksim sesudah_sub;
    leksim_berikut(pos_sub, &sesudah_sub);
    if (sesudah_sub.tipe == TIPE_LEKSIM_KURUNG_BULAT_BUKA) {
      free(hasil);
      return NULL;
    }

    size_t perlu = len + 1 + (size_t)sub.panjang + 1;
    if (perlu > kapasitas) {
      kapasitas = perlu * 2;
      char *baru = realloc(hasil, kapasitas);
      if (!baru) {
        free(hasil);
        return NULL;
      }
      hasil = baru;
    }
    hasil[len++] = '.';
    memcpy(hasil + len, sub.teks, (size_t)sub.panjang);
    len += (size_t)sub.panjang;
    hasil[len] = '\0';

    pos = pos_sub;
  }

  *ptr_masuk = pos;
  return hasil;
}

static PSA *urai_satu_berkas(const char *isi, const char *jalur, bool titik_masuk,
                             bool izinkan_mod) {
  PSA *modul = buat_node(PSA_BERKAS, NULL, 0);
  modul->jalur = salin(jalur);
  modul->titik_masuk = titik_masuk;
  modul->modul = modul_dari_jalur(jalur);

  bool mod_sudah_dideklarasikan = false;
  bool pernyataan_ditemukan = false;

  Leksim leksim;
  const char *ptr = isi;

  while ((ptr = peleksim(ptr, &leksim)) && leksim.tipe != TIPE_LEKSIM_AKHIR) {
    if (leksim.tipe == TIPE_LEKSIM_KOMENTAR)
      continue;

    if (leksim.tipe == TIPE_LEKSIM_KATA_KUNCI_MOD) {
      if (!izinkan_mod || pernyataan_ditemukan) {
        printf(
            "Kesalahan: kata kunci 'mod' harus berada di awal berkas (di %s)\n",
            modul->jalur ? modul->jalur : "?");
        pesan_kesalahan = "kata kunci 'mod' harus berada di awal berkas";
        bersihkan_psa(modul);
        return NULL;
      }

      if (mod_sudah_dideklarasikan) {
        printf("Kesalahan: kata kunci 'mod' hanya boleh dideklarasikan sekali "
               "di awal berkas (di %s)\n",
               modul->jalur ? modul->jalur : "?");
        pesan_kesalahan =
            "kata kunci 'mod' hanya boleh dideklarasikan sekali di awal berkas";
        bersihkan_psa(modul);
        return NULL;
      }

      mod_sudah_dideklarasikan = true;
      PSA *node_mod = buat_node(PSA_MOD, leksim.teks, leksim.panjang);
      node_mod->modul = salin(modul->modul);

      bool satu_baris = false;
      bool modul_pertama = true;

      while (true) {
        Leksim t_cek;
        leksim_berikut(ptr, &t_cek);
        if (t_cek.tipe != TIPE_LEKSIM_PENGENAL)
          break;

        bool ada_baris_baru = false;
        for (const char *c = ptr; c < t_cek.teks; c++) {
          if (*c == '\n') {
            ada_baris_baru = true;
            break;
          }
        }

        if (modul_pertama) {
          satu_baris = !ada_baris_baru;
          modul_pertama = false;
        } else if (satu_baris && ada_baris_baru) {
          break;
        }

        char *nama_mod = urai_pengenal_modul(&ptr);
        if (!nama_mod)
          break;
        PSA *anak_mod =
            buat_node(PSA_PENGENAL, nama_mod, (int)strlen(nama_mod));
        tambah_anak(node_mod, anak_mod);
        free(nama_mod);
      }

      tambah_anak(modul, node_mod);
      continue;
    }

    pernyataan_ditemukan = true;

    if (leksim.tipe == TIPE_LEKSIM_PENGENAL) {
      Leksim lanjut;
      const char *ptr_lanjut = peleksim(ptr, &lanjut);

      if (lanjut.tipe == TIPE_LEKSIM_TITIK) {
        size_t kapasitas = 128;
        char *nama_panggilan = malloc(kapasitas);
        size_t pjg_panggilan = (size_t)leksim.panjang;
        memcpy(nama_panggilan, leksim.teks, pjg_panggilan);
        nama_panggilan[pjg_panggilan] = '\0';

        const char *pos = ptr;
        bool panggilan_valid = false;
        const char *pos_setelah_buka = NULL;

        while (true) {
          Leksim titik;
          const char *pos_titik = peleksim(pos, &titik);
          while (titik.tipe == TIPE_LEKSIM_KOMENTAR)
            pos_titik = peleksim(pos_titik, &titik);
          if (titik.tipe != TIPE_LEKSIM_TITIK)
            break;

          Leksim sub;
          const char *pos_sub = peleksim(pos_titik, &sub);
          while (sub.tipe == TIPE_LEKSIM_KOMENTAR)
            pos_sub = peleksim(pos_sub, &sub);
          if (sub.tipe != TIPE_LEKSIM_PENGENAL)
            break;

          size_t perlu = pjg_panggilan + 1 + (size_t)sub.panjang + 1;
          if (perlu > kapasitas) {
            kapasitas = perlu * 2;
            char *baru = realloc(nama_panggilan, kapasitas);
            if (!baru) {
              free(nama_panggilan);
              bersihkan_psa(modul);
              return NULL;
            }
            nama_panggilan = baru;
          }
          nama_panggilan[pjg_panggilan++] = '.';
          memcpy(nama_panggilan + pjg_panggilan, sub.teks, (size_t)sub.panjang);
          pjg_panggilan += (size_t)sub.panjang;
          nama_panggilan[pjg_panggilan] = '\0';

          Leksim sesudah;
          const char *pos_sesudah = peleksim(pos_sub, &sesudah);
          while (sesudah.tipe == TIPE_LEKSIM_KOMENTAR)
            pos_sesudah = peleksim(pos_sesudah, &sesudah);

          if (sesudah.tipe == TIPE_LEKSIM_KURUNG_BULAT_BUKA) {
            panggilan_valid = true;
            pos_setelah_buka = pos_sesudah;
            break;
          }

          pos = pos_sub;
        }

        if (panggilan_valid) {
          PSA *node = buat_node(PSA_PANGGILAN, nama_panggilan, (int)pjg_panggilan);
          node->modul = salin(modul->modul);
          free(nama_panggilan);
          tambah_anak(modul, node);
          ptr = urai_argumen(pos_setelah_buka, node);
          continue;
        }

        printf(
            "Kesalahan: bertitik hanya didukung dalam panggilan, '%s' (di %s)\n",
            nama_panggilan, modul->jalur ? modul->jalur : "?");
        free(nama_panggilan);
        bersihkan_psa(modul);
        return NULL;
      }

      if (lanjut.tipe == TIPE_LEKSIM_KURUNG_BULAT_BUKA) {
        ptr = ptr_lanjut;

        Leksim lanjut2;
        const char *ptr_lanjut2 = peleksim(ptr, &lanjut2);

        bool adalah_deklarasi = false;
        if (lanjut2.tipe == TIPE_LEKSIM_PENGENAL) {
          adalah_deklarasi = deklarasi_menunggu(ptr_lanjut2);
        } else if (lanjut2.tipe == TIPE_LEKSIM_KURUNG_BULAT_TUTUP) {
          Leksim setelah_tutup;
          peleksim(ptr_lanjut2, &setelah_tutup);
          if (setelah_tutup.tipe == TIPE_LEKSIM_KURUNG_KURAWAT_BUKA ||
              setelah_tutup.tipe == TIPE_LEKSIM_KATA_KUNCI_PUBLIK ||
              setelah_tutup.tipe == TIPE_LEKSIM_KATA_KUNCI_EKSTERNAL ||
              setelah_tutup.tipe == TIPE_LEKSIM_TIPE_DATA_BILANGAN ||
              setelah_tutup.tipe == TIPE_LEKSIM_TIPE_DATA_UNTAIAN) {
            adalah_deklarasi = true;
          }
        }

        if (adalah_deklarasi) {
          PSA *node = buat_node(PSA_DEKLARASI, leksim.teks, leksim.panjang);
          node->modul = salin(modul->modul);
          ptr = ptr_lanjut;

          while ((ptr = peleksim(ptr, &lanjut)) &&
                 lanjut.tipe != TIPE_LEKSIM_KURUNG_BULAT_TUTUP &&
                 lanjut.tipe != TIPE_LEKSIM_AKHIR) {
            if (lanjut.tipe == TIPE_LEKSIM_PENGENAL) {
              tambah_anak(node,
                          buat_node(PSA_PENGENAL, lanjut.teks, lanjut.panjang));
            } else if (lanjut.tipe == TIPE_LEKSIM_TIPE_DATA_UNTAIAN) {
              tambah_anak(node, buat_node(PSA_TIPE_DATA_UNTAIAN, lanjut.teks,
                                          lanjut.panjang));
            } else if (lanjut.tipe == TIPE_LEKSIM_TIPE_DATA_BILANGAN) {
              tambah_anak(node, buat_node(PSA_TIPE_DATA_BILANGAN, lanjut.teks,
                                          lanjut.panjang));
            } else if (lanjut.tipe == TIPE_LEKSIM_KATA_KUNCI_EKSTERNAL) {
              tambah_anak(
                  node, buat_node(PSA_KATA_KUNCI, lanjut.teks, lanjut.panjang));
            } else if (lanjut.tipe == TIPE_LEKSIM_KATA_KUNCI_PUBLIK) {
              tambah_anak(node, buat_node(PSA_KATA_KUNCI_PUBLIK, lanjut.teks,
                                          lanjut.panjang));
            }
          }

          if (lanjut.tipe == TIPE_LEKSIM_KURUNG_BULAT_TUTUP) {
            ptr = peleksim(ptr, &lanjut);

            if (lanjut.tipe == TIPE_LEKSIM_KATA_KUNCI_EKSTERNAL) {
              tambah_anak(
                  node, buat_node(PSA_KATA_KUNCI, lanjut.teks, lanjut.panjang));
            } else if (lanjut.tipe == TIPE_LEKSIM_KATA_KUNCI_PUBLIK) {
              tambah_anak(node, buat_node(PSA_KATA_KUNCI_PUBLIK, lanjut.teks,
                                          lanjut.panjang));
            } else if (lanjut.tipe == TIPE_LEKSIM_TIPE_DATA_UNTAIAN) {
              tambah_anak(node, buat_node(PSA_TIPE_DATA_UNTAIAN, lanjut.teks,
                                          lanjut.panjang));
            } else if (lanjut.tipe == TIPE_LEKSIM_TIPE_DATA_BILANGAN) {
              tambah_anak(node, buat_node(PSA_TIPE_DATA_BILANGAN, lanjut.teks,
                                          lanjut.panjang));
            }
          }

          tambah_anak(modul, node);

          Leksim next;
          const char *ptr_next = peleksim(ptr, &next);
          if (next.tipe == TIPE_LEKSIM_KURUNG_KURAWAT_BUKA) {
            ptr = ptr_next;

            const char *start = ptr;
            int depth = 1;
            const char *inner = ptr;
            Leksim t;
            while (depth > 0 && (inner = peleksim(inner, &t)) &&
                   t.tipe != TIPE_LEKSIM_AKHIR) {
              if (t.tipe == TIPE_LEKSIM_KURUNG_KURAWAT_BUKA)
                depth++;
              else if (t.tipe == TIPE_LEKSIM_KURUNG_KURAWAT_TUTUP)
                depth--;
            }
            size_t len = (size_t)(inner - start);
            char *sub = malloc(len + 1);
            memcpy(sub, start, len);
            sub[len] = '\0';
            PSA *sub_modul = urai_satu_berkas(sub, modul->jalur, false, false);
            if (!sub_modul) {
              free(sub);
              bersihkan_psa(modul);
              return NULL;
            }
            for (int j = 0; j < sub_modul->jumlah_anak; j++) {
              tambah_anak(node, sub_modul->anak[j]);
              sub_modul->anak[j] = NULL;
            }
            bersihkan_psa(sub_modul);
            free(sub);
            ptr = inner;
          }
        } else {
          PSA *node = buat_node(PSA_PANGGILAN, leksim.teks, leksim.panjang);
          node->modul = salin(modul->modul);
          tambah_anak(modul, node);
          ptr = urai_argumen(ptr_lanjut, node);
        }
      } else {
        PSA *node = buat_node(PSA_PENGENAL, leksim.teks, leksim.panjang);
        tambah_anak(modul, node);
      }
    } else if (leksim.tipe == TIPE_LEKSIM_KATA_KUNCI_EKSTERNAL) {
      tambah_anak(modul, buat_node(PSA_KATA_KUNCI, leksim.teks, leksim.panjang));
    } else if (leksim.tipe == TIPE_LEKSIM_KATA_KUNCI_PUBLIK) {
      tambah_anak(modul,
                  buat_node(PSA_KATA_KUNCI_PUBLIK, leksim.teks, leksim.panjang));
    } else if (leksim.tipe == TIPE_LEKSIM_TIPE_DATA_UNTAIAN) {
      tambah_anak(modul,
                  buat_node(PSA_TIPE_DATA_UNTAIAN, leksim.teks, leksim.panjang));
    } else if (leksim.tipe == TIPE_LEKSIM_TIPE_DATA_BILANGAN) {
      tambah_anak(modul,
                  buat_node(PSA_TIPE_DATA_BILANGAN, leksim.teks, leksim.panjang));
    }
  }

  return modul;
}

PSA *urai_banyak_berkas(const char **daftar_jalur, int jumlah_berkas) {
  if (!daftar_jalur || jumlah_berkas <= 0)
    return NULL;

  PSA *akar = buat_node(PSA_PROGRAM, "program", 7);

  for (int i = 0; i < jumlah_berkas; i++) {
    const char *jalur = daftar_jalur[i];
    const char *isi = baca_berkas(jalur);
    if (!isi) {
      printf("Gagal baca: %s\n", jalur);
      bersihkan_psa(akar);
      return NULL;
    }

    PSA *berkas = urai_satu_berkas(isi, jalur, i == 0, true);
    bersihkan_berkas(isi);
    if (!berkas) {
      bersihkan_psa(akar);
      return NULL;
    }

    tambah_anak(akar, berkas);
  }

  return akar;
}

PSA *urai(const char *isi, const char *jalur_berkas) {
  PSA *akar = buat_node(PSA_PROGRAM, "program", 7);
  PSA *berkas = urai_satu_berkas(isi, jalur_berkas, true, true);
  if (!berkas) {
    bersihkan_psa(akar);
    return NULL;
  }
  tambah_anak(akar, berkas);
  return akar;
}