# Nusa

<p align="center">
    <img alt="logo" src="https://raw.githubusercontent.com/bahasa-nusa/lambang/refs/heads/main/lambang.jpg" width="250">
</p>

> Bahasa pemrograman ini masih dalam pengembangan.

## Kebutuhan

- MSYS2 (disarankan untuk lingkungan Windows)
- CMake >= 4.4
- Kompilator C (Clang)
- Ninja

## Cara Build & Fungsi Skrip

> **Catatan:** Skrip-skrip berikut digunakan hanya untuk pengujian (`testing`).

Terdapat 3 skrip build yang tersedia di direktori utama:

1. **`build.sh`**
   - **Fungsi:** Mengonfigurasi proyek menggunakan CMake dengan generator Ninja dan melakukan kompilasi sumber program Nusa.
   - **Cara jalankan:** `./build.sh`
   - **Output:** Menghasilkan direktori `build/` berisi biner utama kompilator `nusa` (atau `nusa.exe` di Windows).

2. **`build.s.sh`**
   - **Fungsi:** Skrip bantu untuk merakit dan menautkan file assembly secara manual (misalnya `titik_masuk.s`, `cetak.s`, `keluar.s`) menggunakan Clang tanpa pustaka standar (`-nostdlib`), menghasilkan biner mentah seperti `halodunia.exe`.
   - **Cara jalankan:** `./build.s.sh`
   - **Output:** File biner `halodunia.exe`.

3. **`build.full.sh`**
   - **Fungsi:** Menjalankan siklus lengkap—mengompilasi kompilator Nusa lewat `build.sh`, menguji kompilasi file sumber `.ns` (seperti `halodunia.ns`), merakit objek, membersihkan file sementara, dan menjalankan eksekusi akhir.
   - **Cara jalankan:** `./build.full.sh`
   - **Output:** Eksekusi program contoh `halodunia.exe` yang langsung dijalankan otomatis setelah proses build selesai.

## Lisensi
Proyek ini dilisensikan di bawah Lisensi Apache 2.0 - lihat [LICENSE](LICENSE) untuk detailnya.