# Scrabble

## Overview

Scrabble is a game project for our university Advanced Programming course UET-INT2215.

## Build instructions

### Prerequisites

- clang++
- CMake
- ninja
- Make sure to specify `clang` as the compiler kit and `ninja` as the makefile generator for CMake

### Build

```
mkdir build
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