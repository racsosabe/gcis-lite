#!/usr/bin/env bash

echo "Cleaning CMake artifacts..."

rm -rf cmake-build-debug
rm -rf build
rm -rf CMakeFiles
rm -f CMakeCache.txt
rm -rf lib
rm -f *.make
rm -f unit_tests/*.cmake

echo "Clean complete."