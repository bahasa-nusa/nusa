#!/bin/sh
clang -c titik_masuk.s -o titik_masuk.o
clang -c cetak.s -o cetak.o
clang -c keluar.s -o keluar.o

clang -nostdlib -Wl,-e,titik_masuk -Wl,--subsystem,console titik_masuk.o cetak.o keluar.o -lkernel32 -o halodunia.exe
