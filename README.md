# Scrabble

## Overview

Scrabble is a game project for our university Advanced Programming course UET-INT2215.

## Build instructions

### Prerequisites

- clang++
- CMake
- ninja

### Build

Make sure you're on the project's root directory.

```
mkdir build

cmake -DCMAKE_C_COMPILER:FILEPATH=clang -DCMAKE_CXX_COMPILER:FILEPATH=clang++ --no-warn-unused-cli -S . -B build -G Ninja

cmake --build build --target all
```

The game executable will be located in `build/bin`

### Launch the game

Windows:
```
.\build\bin\scrabble.exe
```
Linux:
```
./build/bin/scrabble
```