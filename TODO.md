# TODO

> Tugas disini masih dapat berubah.

## Penyesuaian muat berkas (A1)

Buat agar saat muat berkas itu root nya tidak hanya dari berkas titik awal atau berkas utama yang di masukkan, tapi juga dari lokasi biner nusa di simpan (bukan lokasi nusa di eksekusi)/kode/...

Anggap saja ini direktori instalasi nusa
```
kode/standar/cetak.ns
kode/standar/keluar.ns
kode/standar.ns
nusa.exe
```

Anggap saja ini direktori proyek
```
bantu.ns
utama.ns
```

``` utama.ns
'standar.ns'
'bantu.ns'

cetak('tes')
```

## Ganti konsep muat berkas (A2)

Ubah sintaks muat berkas jadi seperti ini.
Base nya itu jadi seperti membuat variabel, tapi dengan tipe data muat
```
nama_variabel muat = 'nama_berkas.ns'
```
terus kalau mau ngambil fungsi atau variabel dari berkas itu, berarti harus lewat nama_variabel nya contoh.
```
std muat = 'standar.ns'

std.cetak('Halo Dunia!')
```

## Perintah mengkompilasi kode ke program atau pustaka (A3)

Perintah nya seperti ini:
``` bash
nusa -kompi <sistem-operasi> <arsitektur> <program|pustaka|objek> <nama-berkas>
```

## Perintah eksekusi kode langsung (A4)

Perintah nya seperti ini:
``` bash
nusa <nama-berkas>
```
