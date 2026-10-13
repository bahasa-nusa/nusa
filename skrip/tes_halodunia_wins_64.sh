#!/bin/sh
./skrip/gubah.sh

./build/nusa.exe llvm opt rkt obj x86_64-pc-windows-msvc contoh/halodunia.ns halodunia.o

./build/so/wins/64/ld.exe \
    -e titik_masuk \
    --subsystem console \
    halodunia.o \
    ./build/so/wins/64/pus/std.a \
    -L./so/wins/64/pus \
    -lkernel32 \
    -o halodunia.exe

rm halodunia.o

./halodunia.exe
echo $?
rm halodunia.exe