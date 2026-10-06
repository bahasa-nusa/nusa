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

1. **`build.sh`**
   - **Fungsi:** Mengonfigurasi proyek menggunakan CMake dengan generator Ninja dan melakukan kompilasi sumber program Nusa.
   - **Cara jalankan:** `./build.sh`
   - **Output:** Menghasilkan direktori `build/` berisi biner utama kompilator `nusa` (atau `nusa.exe` di Windows).

2. **`tes_halodunia_windows_intel_32.sh`** dan **`tes_halodunia_windows_intel_64.sh`**
   - **Fungsi:** Menjalankan siklus lengkap—mengompilasi kompilator Nusa lewat `build.sh`, menguji kompilasi file sumber `.ns` (seperti `halodunia.ns`), merakit objek, membersihkan file sementara, dan menjalankan eksekusi akhir. Versi 32-bit memakai toolchain `so/windows/intel_32`, versi 64-bit memakai `so/windows/intel_64`.
   - **Cara jalankan:** `./tes_halodunia_windows_intel_32.sh` atau `./tes_halodunia_windows_intel_64.sh`
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
$ nusa -i
Penggunaan: nusa <argumen> [berkas]

Opsi:
-v, --versi                                            Untuk melihat versi.
-i, --info                                             Untuk melihat informasi penggunaan.
-tolek                                                 Analisis token.
-urai                                                  Penguraian pohon sintaksis abstrak (PSA).
-smtk                                                  Pemeriksaan semantik.
-ra <sistem-operasi> <arsitektur>                      Representasi Antara.
-opt <sistem-operasi> <arsitektur>                     Optimasi.
-brkt <sistem-operasi> <arsitektur> [-o <berkas>]      Bahasa Rakitan.

Sistem Operasi Yang Tersedia:
  windows
  linux

Arsitektur Yang Tersedia:
  intel_64
  intel_32
```

Flag bisa digabung, contoh `-tolek -urai` mencetak keduanya. `-o <berkas>` hanya berlaku untuk `-brkt`; tanpa `-o`, rakitan dicetak ke konsol.

Contoh keluaran:

```text
$ ./build/nusa -tolek halodunia.ns
Tolek:
Berkas 1: halodunia.ns (titik masuk)
...
```

```text
$ ./build/nusa -brkt windows intel_64 halodunia.ns
Bahasa Rakitan (BRKT):
...
```

## Lisensi
Proyek ini dilisensikan di bawah Lisensi Apache 2.0 - lihat [LICENSE](LICENSE) untuk detailnya.