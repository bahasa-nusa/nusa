#!/bin/sh
./so/windows/intel_32/program/as.exe -c ./so/windows/intel_32/rakitan/ptks.s -o ./so/windows/intel_32/objek/ptks.o
./so/windows/intel_64/program/as.exe -c ./so/windows/intel_64/rakitan/ptks.s -o ./so/windows/intel_64/objek/ptks.o

set -e
cmake -S . -B build -G Ninja
cmake --build build