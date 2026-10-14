#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "nusa/urai_arg.h"

int main(void) {
  {
    char *arg[] = {"nusa", "versi"};
    Arg hasil = urai_arg(2, arg);
    assert(hasil.versi);
    assert(!hasil.info);
    assert(hasil.jumlah_berkas_masuk == 0);
  }

  {
    char *arg[] = {"nusa", "info"};
    Arg hasil = urai_arg(2, arg);
    assert(hasil.info);
    assert(!hasil.versi);
    assert(hasil.jumlah_berkas_masuk == 0);
  }

  {
    char *arg[] = {"nusa", "leks", "a.ns", "b.ns", "c.ns"};
    Arg hasil = urai_arg(5, arg);
    assert(hasil.leks);
    assert(hasil.jumlah_berkas_masuk == 3);
    assert(strcmp(hasil.daftar_berkas_masuk[0], "a.ns") == 0);
    assert(strcmp(hasil.daftar_berkas_masuk[1], "b.ns") == 0);
    assert(strcmp(hasil.daftar_berkas_masuk[2], "c.ns") == 0);
  }

  {
    char *arg[] = {"nusa", "urai", "a.ns", "b.ns"};
    Arg hasil = urai_arg(4, arg);
    assert(hasil.urai);
    assert(hasil.jumlah_berkas_masuk == 2);
    assert(strcmp(hasil.daftar_berkas_masuk[0], "a.ns") == 0);
    assert(strcmp(hasil.daftar_berkas_masuk[1], "b.ns") == 0);
  }

  {
    char *arg[] = {"nusa", "smtk", "contoh/halodunia.ns", "kode/std.ns"};
    Arg hasil = urai_arg(4, arg);
    assert(hasil.smtk);
    assert(hasil.jumlah_berkas_masuk == 2);
    assert(strcmp(hasil.daftar_berkas_masuk[0], "contoh/halodunia.ns") == 0);
    assert(strcmp(hasil.daftar_berkas_masuk[1], "kode/std.ns") == 0);
  }

  {
    char *arg[] = {"nusa", "ra", "opt", "x86_64-pc-windows-msvc", "a.ns", "b.ns"};
    Arg hasil = urai_arg(6, arg);
    assert(hasil.ra);
    assert(hasil.opt);
    assert(hasil.target != NULL && strcmp(hasil.target, "x86_64-pc-windows-msvc") == 0);
    assert(hasil.jumlah_berkas_masuk == 2);
    assert(strcmp(hasil.daftar_berkas_masuk[0], "a.ns") == 0);
    assert(strcmp(hasil.daftar_berkas_masuk[1], "b.ns") == 0);
  }

  {
    char *arg[] = {"nusa", "ra", "x86_64-pc-windows-msvc", "a.ns"};
    Arg hasil = urai_arg(4, arg);
    assert(hasil.ra);
    assert(!hasil.opt);
    assert(hasil.target != NULL && strcmp(hasil.target, "x86_64-pc-windows-msvc") == 0);
    assert(hasil.jumlah_berkas_masuk == 1);
    assert(strcmp(hasil.daftar_berkas_masuk[0], "a.ns") == 0);
  }

  {
    char *arg[] = {"nusa", "llvm", "opt", "rkt", "obj", "x86_64-pc-windows-msvc", "keluar.obj", "a.ns", "b.ns"};
    Arg hasil = urai_arg(9, arg);
    assert(hasil.llvm);
    assert(hasil.opt);
    assert(hasil.bentuk != NULL && strcmp(hasil.bentuk, "rkt") == 0);
    assert(hasil.mode != NULL && strcmp(hasil.mode, "obj") == 0);
    assert(hasil.target != NULL && strcmp(hasil.target, "x86_64-pc-windows-msvc") == 0);
    assert(hasil.berkas_keluar != NULL && strcmp(hasil.berkas_keluar, "keluar.obj") == 0);
    assert(hasil.jumlah_berkas_masuk == 2);
    assert(strcmp(hasil.daftar_berkas_masuk[0], "a.ns") == 0);
    assert(strcmp(hasil.daftar_berkas_masuk[1], "b.ns") == 0);
  }

  {
    char *arg[] = {"nusa", "llvm", "rkt", "obj", "x86_64-pc-windows-msvc"};
    Arg hasil = urai_arg(5, arg);
    assert(hasil.llvm);
    assert(!hasil.opt);
    assert(hasil.bentuk != NULL && strcmp(hasil.bentuk, "rkt") == 0);
    assert(hasil.mode != NULL && strcmp(hasil.mode, "obj") == 0);
    assert(hasil.target != NULL && strcmp(hasil.target, "x86_64-pc-windows-msvc") == 0);
    assert(hasil.berkas_keluar == NULL);
    assert(hasil.jumlah_berkas_masuk == 0);
  }

  {
    char *arg[] = {"nusa", "llvm", "ar", "libstd.a", "a.obj", "b.obj"};
    Arg hasil = urai_arg(6, arg);
    assert(hasil.llvm_ar);
    assert(hasil.berkas_keluar != NULL && strcmp(hasil.berkas_keluar, "libstd.a") == 0);
    assert(hasil.jumlah_berkas_masuk == 2);
    assert(strcmp(hasil.daftar_berkas_masuk[0], "a.obj") == 0);
    assert(strcmp(hasil.daftar_berkas_masuk[1], "b.obj") == 0);
  }

  {
    char *arg[] = {"nusa", "llvm", "ar"};
    Arg hasil = urai_arg(3, arg);
    assert(hasil.llvm_ar);
    assert(hasil.berkas_keluar == NULL);
    assert(hasil.jumlah_berkas_masuk == 0);
  }

  {
    char *arg[] = {"nusa", "llvm", "ra", "teks", "x86_64-pc-windows-msvc", "keluar.ll", "a.ns"};
    Arg hasil = urai_arg(7, arg);
    assert(hasil.llvm);
    assert(!hasil.opt);
    assert(hasil.bentuk != NULL && strcmp(hasil.bentuk, "ra") == 0);
    assert(hasil.mode != NULL && strcmp(hasil.mode, "teks") == 0);
    assert(hasil.target != NULL && strcmp(hasil.target, "x86_64-pc-windows-msvc") == 0);
    assert(hasil.berkas_keluar != NULL && strcmp(hasil.berkas_keluar, "keluar.ll") == 0);
    assert(hasil.jumlah_berkas_masuk == 1);
    assert(strcmp(hasil.daftar_berkas_masuk[0], "a.ns") == 0);
  }

  {
    char *arg[] = {"nusa"};
    Arg hasil = urai_arg(1, arg);
    assert(!hasil.versi);
    assert(!hasil.info);
    assert(!hasil.leks);
    assert(!hasil.urai);
    assert(!hasil.smtk);
    assert(!hasil.ra);
    assert(!hasil.opt);
    assert(!hasil.llvm);
    assert(!hasil.llvm_ar);
    assert(hasil.target == NULL);
    assert(hasil.bentuk == NULL);
    assert(hasil.mode == NULL);
    assert(hasil.berkas_keluar == NULL);
    assert(hasil.daftar_berkas_masuk == NULL);
    assert(hasil.jumlah_berkas_masuk == 0);
  }

  printf("OK\n");
  return 0;
}

