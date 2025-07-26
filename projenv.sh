#!/usr/bin/env bash

build() {
  target=$1
  if [ ! -d build ]; then
    mkdir build
    cmake -DCMAKE_C_COMPILER:FILEPATH=clang -DCMAKE_CXX_COMPILER:FILEPATH=clang++ --no-warn-unused-cli -S . -B build -G Ninja
  fi
  if [ -z "$target" ]; then
    cmake --build build --config Debug --target all --
  else
    cmake --build build --config Debug --target "$target"
  fi
}

scrabble() {
  build scrabble
  exec_path="build/bin/scrabble"
  if [ -x "$exec_path" ]; then
    "$exec_path" "$1" "$2" "$3"
  else
    echo "Error: '$exec_path' not found"
  fi
}

test() {
  build scrabble_test
  exec_path="build/bin/scrabble_test"
  if [ -x "$exec_path" ]; then
    "$exec_path" "$1" "$2" "$3"
  else
    echo "Error: '$exec_path' not found"
  fi
}

case "$1" in
  build)
    shift
    build "$@"
    ;;
  scrabble)
    shift
    scrabble "$@"
    ;;
  test)
    shift
    test "$@"
    ;;
  *)
    echo "Usage: $0 {build [target]|scrabble [args]|test [args]}"
    exit 1
    ;;
esac
