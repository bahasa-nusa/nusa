#!/bin/sh
./gubah.sh

./build/nusa.exe rkt wins 64 contoh/halodunia.ns halodunia.s

./so/wins/64/as.exe -c halodunia.s -o halodunia.o

./so/wins/64/ld.exe \
    -e titik_masuk \
    --subsystem console \
    halodunia.o \
    ./so/wins/64/pus/std.a \
    -L./so/wins/64/pus \
    -lkernel32 \
    -o halodunia.exe

rm halodunia.o halodunia.s
clear

./halodunia.exe
echo $?
rm halodunia.exe