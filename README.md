# Scrabble

## Tổng quan

Scrabble là dự án game cho môn học Lập trình nâng cao INT2215 kì hè 2024-2025

## Hướng dẫn build và chạy game

### Yêu cầu

Hệ thống người dùng cần có sẵn các công cụ sau:

- clang++
- CMake
- ninja

3 công cụ này có thể được cài đơn giản qua môi trường MSYS2

### Build

cd vào thư mục root của project, sau đó chạy lệnh:

```
mkdir build

cmake -DCMAKE_C_COMPILER:FILEPATH=clang -DCMAKE_CXX_COMPILER:FILEPATH=clang++ --no-warn-unused-cli -S . -B build -G Ninja

cmake --build build --target scrabblle
```

Sau khi build, file executable của game sẽ nằm trong thư mục build/bin/scrabble(.exe)

### Chạy game

Lưu ý: Để game nhận assets thành công, đường dẫn khi chạy game (working directory) phải nằm ở thư mục root của project. Ví dụ khi project được clone về tại projects/scrabble, lệnh chạy sẽ như sau:

Windows:
```
cd projects/scrabble
.\build\bin\scrabble.exe
```
Linux:
```
cd projects/scrabble
./build/bin/scrabble
```