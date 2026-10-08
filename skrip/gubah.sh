#!/bin/sh
# Bangun pus std.a (Windows 32)
./so/wins/32/as.exe -c ./so/wins/32/rkt/std/keluar.s -o ./so/wins/32/obj/std/keluar.o
./so/wins/32/as.exe -c ./so/wins/32/rkt/std/cetak.s -o ./so/wins/32/obj/std/cetak.o

./so/wins/32/ar rcs ./so/wins/32/pus/std.a ./so/wins/32/obj/std/keluar.o ./so/wins/32/obj/std/cetak.o

# Bangun pus std.a (Windows 64)
./so/wins/64/as.exe -c ./so/wins/64/rkt/std/keluar.s -o ./so/wins/64/obj/std/keluar.o
./so/wins/64/as.exe -c ./so/wins/64/rkt/std/cetak.s -o ./so/wins/64/obj/std/cetak.o

./so/wins/64/ar rcs ./so/wins/64/pus/std.a ./so/wins/64/obj/std/keluar.o ./so/wins/64/obj/std/cetak.o

# Bangun cmake project
set -e
cmake -S . -B build -G Ninja
cmake --build build