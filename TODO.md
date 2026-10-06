# TODO

> Tugas disini masih dapat berubah.

## Perintah terminal untuk membuat project

Perintah nya seperti ini:
``` bash
nusa inis
nusa inis .
nusa inis <nama-direktori>
```

Stuktur proyek yg di buat:
```
# Salinan pustaka standar
kode/pustaka/standar/*.ns
kode/pustaka/standar.ns

# Membuat berkas utama
kode/utama.ns
```

Kode berkas utama.ns pertamakali:
```
'pustaka/standar.ns'

cetak('Halo Dunia!')
```

## Perintah eksekusi kode langsung

Perintah nya seperti ini:
``` bash
nusa <nama-berkas>
```

## Perintah mengkompilasi kode ke program atau pustaka

Perintah nya seperti ini:
``` bash
nusa -kompi <sistem-operasi> <arsitektur> <program|pustaka|objek> <nama-berkas>
```