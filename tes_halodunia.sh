#!/bin/sh
./build.sh

./build/nusa.exe -brkt windows intel_64 halodunia.ns -o halodunia.s

clang -c halodunia.s -o halodunia.o
clang -c cetak.s -o cetak.o
clang -c keluar.s -o keluar.o

clang -nostdlib -Wl,-e,titik_masuk -Wl,--subsystem,console halodunia.o cetak.o keluar.o -lkernel32 -o halodunia.exe

rm cetak.o keluar.o halodunia.o halodunia.s
clear

./halodunia.exe
rm halodunia.exe