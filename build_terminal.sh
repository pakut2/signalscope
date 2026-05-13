#!/bin/zsh

cmake -S terminal -B build
cmake --build build

./build/signalscope.app/Contents/MacOS/signalscope

