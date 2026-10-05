#include "nusa/penolek.h"
#include <ctype.h>

const char* nama_tolek(TipeTolek tipe) {
    switch (tipe) {
        case TIPE_TOLEK_AKHIR: return "AKHIR";
        case TIPE_TOLEK_KOMENTAR: return "COMMENT";
        case TIPE_TOLEK_UNTAIAN: return "UNTAIAN";
        case TIPE_TOLEK_PENGENAL: return "PENGENAL";
        case TIPE_TOLEK_KURUNG_BULAT_BUKA: return "KURUNG BULAT BUKA";
        case TIPE_TOLEK_KURUNG_BULAT_TUTUP: return "KURUNG BULAT TUTUP";
        default: return "TIDAK DIKETAHUI";
    }
}

const char* penolek(const char* isi, Tolek* hasil) {
    while (*isi && isspace((unsigned char)*isi) && *isi != '\n') isi++;
    
    if (!*isi) {
        hasil->tipe = TIPE_TOLEK_AKHIR;
        hasil->teks = isi;
        hasil->panjang = 0;
        return isi;
    }

    if (*isi == '\n') {
        isi++;
        return penolek(isi, hasil);
    }

    if (*isi == '#') {
        const char* awal = isi;
        while (*isi && *isi != '\n' && *isi != '\r') isi++;
        hasil->tipe = TIPE_TOLEK_KOMENTAR;
        hasil->teks = awal;
        hasil->panjang = (int)(isi - awal);
        return isi;
    }

    if (*isi == '\'') {
        const char* awal = isi++;
        while (*isi && *isi != '\'' && *isi != '\n') isi++;
        if (*isi == '\'') isi++;

        hasil->tipe = TIPE_TOLEK_UNTAIAN;
        hasil->teks = awal;
        hasil->panjang = (int)(isi - awal);
        return isi;
    }

    if (isalpha((unsigned char)*isi) || *isi == '_') {
        const char* awal = isi;
        while (*isi && (isalnum((unsigned char)*isi) || *isi == '_')) isi++;
        hasil->tipe = TIPE_TOLEK_PENGENAL;
        hasil->teks = awal;
        hasil->panjang = (int)(isi - awal);
        return isi;
    }

    if (*isi == '(') {
        hasil->tipe = TIPE_TOLEK_KURUNG_BULAT_BUKA;
        hasil->teks = isi;
        hasil->panjang = 1;
        return isi + 1;
    }

    if (*isi == ')') {
        hasil->tipe = TIPE_TOLEK_KURUNG_BULAT_TUTUP;
        hasil->teks = isi;
        hasil->panjang = 1;
        return isi + 1;
    }

    hasil->tipe = TIPE_TOLEK_TIDAK_DIKETAHUI;
    hasil->teks = isi;
    hasil->panjang = 1;
    return isi + 1;
}