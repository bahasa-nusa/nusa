#!/bin/sh
./build.sh
./build/nusa.exe halodunia.ns
./build.s.sh

rm cetak.o keluar.o titik_masuk.o titik_masuk.s
clear

./halodunia.exe
rm halodunia.exe
