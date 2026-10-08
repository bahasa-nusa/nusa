#!/bin/sh
./build.sh

./build/nusa.exe rkt wins intel32 contoh/halodunia.ns halodunia.s

./so/wins/intel32/as.exe -c halodunia.s -o halodunia.o
./so/wins/intel32/as.exe -c cetak_tergantung_os_dan_arsitektur_ns.s -o cetak_tergantung_os_dan_arsitektur_ns.o

./so/wins/intel32/ld.exe \
    -e titik_masuk \
    --subsystem console \
    halodunia.o \
    cetak_tergantung_os_dan_arsitektur_ns.o \
    ./so/wins/intel32/pus/std.a \
    -L./so/wins/intel32/pus \
    -lkernel32 \
    -o halodunia.exe

rm halodunia.o cetak_tergantung_os_dan_arsitektur_ns.o halodunia.s cetak_tergantung_os_dan_arsitektur_ns.s
clear

./halodunia.exe
echo $?
rm halodunia.exe