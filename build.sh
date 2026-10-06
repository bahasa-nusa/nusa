#!/bin/sh
# Bangun pustaka ptks.a (Windows Intel 32)
./so/windows/intel_32/program/as.exe -c ./so/windows/intel_32/rakitan/ptks/keluar.s -o ./so/windows/intel_32/objek/ptks/keluar.o
./so/windows/intel_32/program/as.exe -c ./so/windows/intel_32/rakitan/ptks/cetak.s -o ./so/windows/intel_32/objek/ptks/cetak.o

./so/windows/intel_32/program/ar rcs ./so/windows/intel_32/pustaka/ptks.a ./so/windows/intel_32/objek/ptks/keluar.o ./so/windows/intel_32/objek/ptks/cetak.o

# Bangun pustaka ptks.a (Windows Intel 64)
./so/windows/intel_64/program/as.exe -c ./so/windows/intel_64/rakitan/ptks/keluar.s -o ./so/windows/intel_64/objek/ptks/keluar.o
./so/windows/intel_64/program/as.exe -c ./so/windows/intel_64/rakitan/ptks/cetak.s -o ./so/windows/intel_64/objek/ptks/cetak.o

./so/windows/intel_64/program/ar rcs ./so/windows/intel_64/pustaka/ptks.a ./so/windows/intel_64/objek/ptks/keluar.o ./so/windows/intel_64/objek/ptks/cetak.o

# Bangun cmake project
set -e
cmake -S . -B build -G Ninja
cmake --build build