#!/bin/sh
# Bangun cmake project
set -e
cmake -S . -B build -G Ninja
cmake --build build
cmake --install build --prefix build