#!/bin/sh
./build.sh

./build/nusa.exe rkt wins intel64 contoh/halodunia.ns halodunia.s

./so/wins/intel64/as.exe -c halodunia.s -o halodunia.o
./so/wins/intel64/as.exe -c cetak_tergantung_os_dan_arsitektur_ns.s -o cetak_tergantung_os_dan_arsitektur_ns.o

./so/wins/intel64/ld.exe \
    -e titik_masuk \
    --subsystem console \
    halodunia.o \
    cetak_tergantung_os_dan_arsitektur_ns.o \
    ./so/wins/intel64/pus/std.a \
    -L./so/wins/intel64/pus \
    -lkernel32 \
    -o halodunia.exe

rm halodunia.o cetak_tergantung_os_dan_arsitektur_ns.o halodunia.s cetak_tergantung_os_dan_arsitektur_ns.s
clear

./halodunia.exe
echo $?
rm halodunia.exe