#!/bin/sh
./build.sh

./build/nusa.exe -brkt windows intel_32 kode/halodunia.ns -o halodunia.s

./so/windows/intel_32/program/as.exe -c halodunia.s -o halodunia.o

./so/windows/intel_32/program/ld.exe \
    -e titik_masuk \
    --subsystem console \
    halodunia.o \
    ./so/windows/intel_32/pustaka/standar.a \
    -L./so/windows/intel_32/pustaka \
    -lkernel32 \
    -o halodunia.exe

rm halodunia.o halodunia.s
clear

./halodunia.exe
echo $?
rm halodunia.exe