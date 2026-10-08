#!/bin/sh
# Bangun pus std.a (Windows Intel 32)
./so/wins/intel32/as.exe -c ./so/wins/intel32/rkt/std/keluar.s -o ./so/wins/intel32/obj/std/keluar.o
./so/wins/intel32/as.exe -c ./so/wins/intel32/rkt/std/cetak.s -o ./so/wins/intel32/obj/std/cetak.o

./so/wins/intel32/ar rcs ./so/wins/intel32/pus/std.a ./so/wins/intel32/obj/std/keluar.o ./so/wins/intel32/obj/std/cetak.o

# Bangun pus std.a (Windows Intel 64)
./so/wins/intel64/as.exe -c ./so/wins/intel64/rkt/std/keluar.s -o ./so/wins/intel64/obj/std/keluar.o
./so/wins/intel64/as.exe -c ./so/wins/intel64/rkt/std/cetak.s -o ./so/wins/intel64/obj/std/cetak.o

./so/wins/intel64/ar rcs ./so/wins/intel64/pus/std.a ./so/wins/intel64/obj/std/keluar.o ./so/wins/intel64/obj/std/cetak.o

# Bangun cmake project
set -e
cmake -S . -B build -G Ninja
cmake --build build