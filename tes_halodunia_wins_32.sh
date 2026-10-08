#!/bin/sh
./build.sh

./build/nusa.exe rkt wins 32 contoh/halodunia.ns halodunia.s

./so/wins/32/as.exe -c halodunia.s -o halodunia.o

./so/wins/32/ld.exe \
    -e titik_masuk \
    --subsystem console \
    halodunia.o \
    ./so/wins/32/pus/std.a \
    -L./so/wins/32/pus \
    -lkernel32 \
    -o halodunia.exe

rm halodunia.o halodunia.s
clear

./halodunia.exe
echo $?
rm halodunia.exe