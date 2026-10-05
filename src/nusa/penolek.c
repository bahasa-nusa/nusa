#include "nusa/penolek.h"
#include <ctype.h>
#include <string.h>

const char* nama_tolek(TipeTolek tipe) {
    switch (tipe) {
        case TIPE_TOLEK_AKHIR: return "AKHIR";
        case TIPE_TOLEK_KOMENTAR: return "COMMENT";
        case TIPE_TOLEK_NILAI_UNTAIAN: return "NILAI UNTAIAN";
        case TIPE_TOLEK_NILAI_BILANGAN: return "NILAI BILANGAN";
        case TIPE_TOLEK_PENGENAL: return "PENGENAL";
        case TIPE_TOLEK_KURUNG_BULAT_BUKA: return "KURUNG BULAT BUKA";
        case TIPE_TOLEK_KURUNG_BULAT_TUTUP: return "KURUNG BULAT TUTUP";
        case TIPE_TOLEK_KATA_KUNCI_EKSTERNAL: return "KATA KUNCI EKSTERNAL";
        case TIPE_TOLEK_TIPE_DATA_BILANGAN: return "TIPE DATA B32";
        case TIPE_TOLEK_TIPE_DATA_UNTAIAN: return "TIPE DATA UNTAIAN";
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

        hasil->tipe = TIPE_TOLEK_NILAI_UNTAIAN;
        hasil->teks = awal;
        hasil->panjang = (int)(isi - awal);
        return isi;
    }

    if (isdigit((unsigned char)*isi)) {
        const char* awal = isi;
        while (*isi && isdigit((unsigned char)*isi)) isi++;
        hasil->tipe = TIPE_TOLEK_NILAI_BILANGAN;
        hasil->teks = awal;
        hasil->panjang = (int)(isi - awal);
        return isi;
    }

    if (isalpha((unsigned char)*isi)) {
        const char* awal = isi;

        if (strncmp(isi, "eks", 3) == 0 && (isi[3] == '\0' || isspace((unsigned char)isi[3]) || isi[3] == '(' || isi[3] == ')')) {
            hasil->tipe = TIPE_TOLEK_KATA_KUNCI_EKSTERNAL;
            hasil->teks = awal;
            hasil->panjang = 3;
            return isi + 3;
        }

        if (strncmp(isi, "unt", 3) == 0 && (isi[3] == '\0' || isspace((unsigned char)isi[3]) || isi[3] == '(' || isi[3] == ')')) {
            hasil->tipe = TIPE_TOLEK_TIPE_DATA_UNTAIAN;
            hasil->teks = awal;
            hasil->panjang = 3;
            return isi + 3;
        }

        if (strncmp(isi, "b32", 3) == 0 && (isi[3] == '\0' || isspace((unsigned char)isi[3]) || isi[3] == '(' || isi[3] == ')')) {
            hasil->tipe = TIPE_TOLEK_TIPE_DATA_BILANGAN;
            hasil->teks = awal;
            hasil->panjang = 3;
            return isi + 3;
        }



        while (*isi && (isalnum((unsigned char)*isi) || *isi == '_')) isi++;
        hasil->tipe = TIPE_TOLEK_PENGENAL;
        hasil->teks = awal;
        hasil->panjang = (int)(isi - awal);
        return isi;
    }

    if (*isi == '_') {
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