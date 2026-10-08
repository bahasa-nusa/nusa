# Nusa

<p align="center">
    <img alt="logo" src="https://raw.githubusercontent.com/bahasa-nusa/lambang/refs/heads/main/lambang.jpg" width="250">
</p>

> Bahasa pemrograman ini masih dalam pengembangan.

## Cara Build

### Kebutuhan

- MSYS2 (Windows)
- Kompilator C (Clang v22, kompiler lain belom dicoba)
- CCache
- CMake >= 4.4
- Ninja

### Fungsi Skrip

> **Catatan:** Skrip-skrip berikut digunakan hanya untuk pengujian (`testing`).

Terdapat 3 skrip yang tersedia di direktori utama:

1. **`gubah.sh`**
   - **Fungsi:** Mengonfigurasi proyek menggunakan CMake dengan generator Ninja dan melakukan kompilasi sumber program Nusa.
   - **Cara jalankan:** `./skrip/gubah.sh`
   - **Output:** Menghasilkan direktori `build/` berisi biner utama kompilator `nusa` (atau `nusa.exe` di Windows).

2. **`tes_halodunia_wins_32.sh`** dan **`tes_halodunia_wins_64.sh`**
   - **Fungsi:** Menjalankan siklus lengkap—mengompilasi kompilator Nusa lewat `skrip/gubah.sh`, menguji kompilasi file sumber `.ns` (seperti `halodunia.ns`), merakit objek, membersihkan file sementara, dan menjalankan eksekusi akhir. Versi 32-bit memakai toolchain `so/wins/32`, versi 64-bit memakai `so/wins/64`.
   - **Cara jalankan:** `./skrip/tes_halodunia_wins_32.sh` atau `./skrip/tes_halodunia_wins_64.sh`
   - **Output:** Eksekusi program contoh `halodunia.exe` yang langsung dijalankan otomatis setelah proses build selesai.

## Cara Install

Setelah build berhasil, install ke direktori tujuan dengan `cmake --install`:

```sh
cmake --install build --prefix <direktori_tujuan>
```

Contoh (install ke `/c/nusa`):

```sh
cmake --install build --prefix /c/nusa
```

Yang terinstall:
- Biner kompilator
- Kode pustaka bawaan Nusa
- Alat dan pustaka yang dibutuhkan sistem operasi tertentu
- LICENSE

## Cara Pakai

```sh
$ nusa info
Penggunaan: nusa <perintah> [berkas]

Perintah:
  versi                                                         Untuk melihat versi.
  info                                                          Untuk melihat informasi penggunaan.
  leks <berkas>                                                 Analisis leksim.
  urai <berkas>                                                 Penguraian pohon sintaksis abstrak (PSA).
  smtk <berkas>                                                 Pemeriksaan semantik.
  ra <sistem-operasi> <arsitektur> <berkas>                     Representasi antara.
  opt <sistem-operasi> <arsitektur> <berkas>                    Optimasi.
  rkt <sistem-operasi> <arsitektur> <berkas> [<berkas-keluar>]  Bahasa rakitan.

Sistem Operasi:
  wins
  linux

Arsitektur:
  64
  32

```

Contoh keluaran:

```text
$ nusa leks halodunia.ns
Leksim:
Berkas 1: halodunia.ns (titik masuk)
...
```

```text
$ nusa rkt wins 64 halodunia.ns
Bahasa Rakitan (RKT):
...
```

## Lisensi
Proyek ini dilisensikan di bawah Lisensi Apache 2.0 - lihat [LICENSE](LICENSE) untuk detailnya.