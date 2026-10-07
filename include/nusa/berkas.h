// Copyright 2026 Pengembang Bahasa Pemrograman Nusa
// SPDX-License-Identifier: Apache-2.0

#ifndef NUSA_BERKAS_H
#define NUSA_BERKAS_H

const char *baca_berkas(const char *nama_berkas);

void bersihkan_berkas(const char *isi_berkas);

bool berkas_sudah_dimuat(const char *jalur_resolusi);

void tandai_berkas_dimuat(const char *jalur_resolusi);

void bersihkan_daftar_dimuat();

char *gabung_jalur_relatif(const char *dasar, const char *jalur);

char *jalur_kanonis(const char *jalur);

extern char *direktori_berkas_utama;
extern char *direktori_instalasi;

char *direktori_dari(const char *jalur);

char *jalur_biner(void);

char *cari_berkas(const char *nama);

#endif
