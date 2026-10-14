#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nusa/berkas.h"
#include "nusa/optimasi.h"
#include "nusa/pengurai.h"
#include "nusa/peleksim.h"
#include "nusa/pesemantik.h"
#include "nusa/target.h"

#include "nusa/urai_arg.h"
#include "nusa/bagkang_llvm.h"
#include "nusa/arsip_llvm.h"
#include "nusa/jembat.h"

void cetak_info() {
  printf("Penggunaan: nusa <perintah> [berkas]\n\n");

  printf("Perintah:\n");
  printf("  versi                                                               Untuk melihat versi.\n");
  printf("  info                                                                Untuk melihat informasi penggunaan.\n");
  printf("  leks <berkas>                                                       Analisis leksim.\n");
  printf("  urai <berkas>                                                       Penguraian pohon sintaksis abstrak (PSA).\n");
  printf("  smtk <berkas>                                                       Pemeriksaan semantik.\n");
  printf("  ra [opt] <target> <berkas>                                          Representasi antara.\n");
  printf("  llvm [opt] <ra|rkt> <teks|obj> <target> <berkas> [<berkas-keluar>]  Backend LLVM.\n");
  printf("  llvm ar <berkas-keluar> <nama-berkas>*                              Buat arsip pustaka statis (.a).\n");
  printf("\n");
  printf("  Kombinasi llvm:\n");
  printf("    llvm ra teks  -> LLVM IR tekstual (.ll)\n");
  printf("    llvm rkt teks -> Assembly tekstual (.s)\n");
  printf("    llvm ra obj   -> Bitcode biner (.bc)\n");
  printf("    llvm rkt obj  -> Objek biner (.o Linux, .obj Windows)\n");
}

int main(int argc, char **argv) {
  Arg arg = urai_arg(argc, argv);

  if (arg.versi) {
    printf("nusa v0.0.0\n");
    return 0;
  }

  if (arg.info) {
    cetak_info();
    return 0;
  }

  if (arg.llvm_ar) {
    if (!arg.berkas_keluar || arg.jumlah_berkas_masuk == 0) {
      fprintf(stderr, "Penggunaan: nusa llvm ar <berkas-keluar> <nama-berkas>*\n");
      return 1;
    }
    return bagkang_llvm_buat_arsip(arg.berkas_keluar,
                                   arg.daftar_berkas_masuk,
                                   arg.jumlah_berkas_masuk);
  }

  if (arg.leks) {
    if (arg.jumlah_berkas_masuk == 0) {
      printf("Argumen tidak valid.\n");
      cetak_info();
      return 1;
    }
    printf("Leksim:\n");
    for (int i = 0; i < arg.jumlah_berkas_masuk; i++) {
      if (i > 0)
        printf("\n");
      printf("Berkas %d: %s%s\n", i + 1, arg.daftar_berkas_masuk[i],
             i == 0 ? " (titik masuk)" : "");

      const char *isi = baca_berkas(arg.daftar_berkas_masuk[i]);
      if (!isi) {
        printf("Gagal baca: %s\n", arg.daftar_berkas_masuk[i]);
        return 1;
      }

      int max_len = 0;
      const char *ptr = isi;
      Leksim leksim;
      while ((ptr = peleksim(ptr, &leksim)) && leksim.tipe != TIPE_LEKSIM_AKHIR) {
        if (leksim.panjang > max_len)
          max_len = leksim.panjang;
      }
      if (max_len < 22)
        max_len = 22;

      ptr = isi;
      while ((ptr = peleksim(ptr, &leksim)) && leksim.tipe != TIPE_LEKSIM_AKHIR) {
        printf("%*.*s | %s\n", max_len, leksim.panjang, leksim.teks,
               nama_leksim(leksim.tipe));
      }
      printf("%*s | %s\n", max_len, "", nama_leksim(TIPE_LEKSIM_AKHIR));

      bersihkan_berkas(isi);
    }
    return 0;
  }

  const char *berkas_masuk = arg.jumlah_berkas_masuk > 0 ? arg.daftar_berkas_masuk[0] : NULL;
  if (berkas_masuk) {
    if (!arg.urai && !arg.smtk && !arg.ra && !arg.opt && !arg.llvm) {
      printf("Argumen tidak valid.\n");
      cetak_info();
      return 1;
    }

    direktori_berkas_utama = direktori_dari(berkas_masuk);

    char *biner = jalur_biner();
    direktori_instalasi = direktori_dari(biner ? biner : argv[0]);
    free(biner);

    const char *isi_berkas = baca_berkas(berkas_masuk);
    if (!isi_berkas) {
      printf("Gagal baca: %s\n", berkas_masuk);
      return 1;
    }

    if (arg.urai || arg.smtk || arg.ra || arg.opt || arg.llvm) {
      if (arg.urai) {
        printf("PSA:\n");
      }
      bersihkan_daftar_dimuat();
      tandai_berkas_dimuat(berkas_masuk);

      PSA *psa = urai(isi_berkas, berkas_masuk);
      if (psa) {
        if (arg.urai) {
          cetak_psa(psa, 0);
        }
        if (arg.smtk) {
          printf("\nPesemantik:\n");
          int galat = pesemantik(psa);
          if (!galat)
            printf("Tidak ada kesalahan\n");
          else
            return 1;
        }
        if (arg.ra || arg.opt) {
          if (!arg.target) {
            printf("Perintah ra memerlukan <target>\n");
            return 1;
          }
          const Target *t = set_target(arg.target);
          if (!t) {
            return 1;
          }
          InstruksiRA *ra = bangkitkan_ra(psa);
          InstruksiRA *ra_imp = tambah_impor_ra(ra, ra);
          if (arg.ra) {
            if (arg.opt) {
              InstruksiRA *ra_opt = optimalkan(ra);
              InstruksiRA *ra_opt_imp = tambah_impor_ra(ra_opt, ra_opt);
              printf("\nOptimasi:\n");
              cetak_ra_permodul(ra_opt_imp);
              bersihkan_ra(ra_imp);
              bersihkan_ra(ra_opt_imp);
            } else {
              printf("\nRA:\n");
              cetak_ra_permodul(ra_imp);
              bersihkan_ra(ra_imp);
            }
          }
        }

        if (arg.llvm) {
          if (!arg.target) {
            printf("Perintah llvm memerlukan <target>\n");
            return 1;
          }
          if (!arg.bentuk || (strcmp(arg.bentuk, "ra") != 0 && strcmp(arg.bentuk, "rkt") != 0)) {
            printf("Perintah llvm memerlukan <ra|rkt>\n");
            return 1;
          }
          if (!arg.mode || (strcmp(arg.mode, "teks") != 0 && strcmp(arg.mode, "obj") != 0)) {
            printf("Perintah llvm memerlukan <teks|obj>\n");
            return 1;
          }
          if (!bagkang_llvm_target_tersedia(arg.target)) {
            printf("Target tidak didukung LLVM: %s\n", arg.target);
            return 1;
          }
          
          bagkang_llvm_daftarkan();

          InstruksiRA *ra = bangkitkan_ra(psa);
          InstruksiRA *ra_imp = tambah_impor_ra(ra, ra);
          InstruksiRA *ra_opt = optimalkan(ra);
          InstruksiRA *ra_opt_imp = tambah_impor_ra(ra_opt, ra_opt);

          OpsiBackend opsi = {
            .optimasi = arg.opt,
            .so = NULL,
            .target = arg.target,
            .bentuk = arg.bentuk,
            .mode = arg.mode,
            .berkas_keluar = arg.berkas_keluar,
            .berkas_masuk = berkas_masuk
          };

          int hasil = jembat_jalankan("llvm", ra_opt_imp, &opsi);
          bersihkan_ra(ra_imp);
          bersihkan_ra(ra_opt_imp);

          if (hasil != 0) {
            return 1;
          }
        }
        
        bersihkan_psa(psa);
      } else {
        printf("Gagal mengurai: %s\n", pesan_urai());
      }
    }

    bersihkan_daftar_dimuat();
    bersihkan_berkas(isi_berkas);
  } else {
    if (argc > 1)
      printf("Argumen tidak valid.\n");
    cetak_info();
  }

  return 0;
}