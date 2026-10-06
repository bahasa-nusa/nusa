#!/bin/sh
# Bangun pustaka standar.a (Windows Intel 32)
./so/windows/intel_32/as.exe -c ./so/windows/intel_32/rakitan/standar/keluar.s -o ./so/windows/intel_32/objek/standar/keluar.o
./so/windows/intel_32/as.exe -c ./so/windows/intel_32/rakitan/standar/cetak.s -o ./so/windows/intel_32/objek/standar/cetak.o

./so/windows/intel_32/ar rcs ./so/windows/intel_32/pustaka/standar.a ./so/windows/intel_32/objek/standar/keluar.o ./so/windows/intel_32/objek/standar/cetak.o

# Bangun pustaka standar.a (Windows Intel 64)
./so/windows/intel_64/as.exe -c ./so/windows/intel_64/rakitan/standar/keluar.s -o ./so/windows/intel_64/objek/standar/keluar.o
./so/windows/intel_64/as.exe -c ./so/windows/intel_64/rakitan/standar/cetak.s -o ./so/windows/intel_64/objek/standar/cetak.o

./so/windows/intel_64/ar rcs ./so/windows/intel_64/pustaka/standar.a ./so/windows/intel_64/objek/standar/keluar.o ./so/windows/intel_64/objek/standar/cetak.o

# Bangun cmake project
set -e
cmake -S . -B build -G Ninja
cmake --build build