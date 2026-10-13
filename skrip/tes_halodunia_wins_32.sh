#!/bin/sh
./skrip/gubah.sh

./build/nusa.exe llvm opt rkt obj i686-pc-windows-msvc contoh/halodunia.ns halodunia.o

./build/so/wins/32/ld.exe \
    -e titik_masuk \
    --subsystem console \
    halodunia.o \
    ./build/so/wins/32/pus/std.a \
    -L./so/wins/32/pus \
    -lkernel32 \
    -o halodunia.exe

rm halodunia.o

./halodunia.exe
echo $?
rm halodunia.exe