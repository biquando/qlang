# QLang

This project provides two libraries: `lexer` and `parser`.

## Building

Dependencies:

- cmake 3.21+

```bash
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
# OR: cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build
cmake --build build -j8
cd build
# OPTIONAL: ctest
cpack
```
