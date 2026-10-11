#!/bin/sh
./skrip/gubah.sh

./build/nusa.exe llvm opt rkt teks x86_64-pc-windows-msvc contoh/halodunia.ns halodunia.s

./build/so/wins/64/as.exe -c halodunia.s -o halodunia.o

./build/so/wins/64/ld.exe \
    -e titik_masuk \
    --subsystem console \
    halodunia.o \
    ./build/so/wins/64/pus/std.a \
    -L./so/wins/64/pus \
    -lkernel32 \
    -o halodunia.exe

rm halodunia.o halodunia.s

./halodunia.exe
echo $?
rm halodunia.exe