# gccgc

## Dynamic Linking

Linking to `gccgc`:

```bash
# 1) include and use gccgc in your project
# 2) generate object file for your project
gcc -I ../path/to/gccgc -c main.c -o main.o
# 3) generate shared object file
make
# 4) link your project to gccgc
gcc -o main main.o -L../path/to/gccgc -lgccgc
# you may need to add the lib location to your PATH
```

Linking to `gccgc` on Windows:

```bash
# 1) include and use gccgc in your project
# 2) generate object file for your project
gcc -I ../path/to/gccgc -c main.c -o main.o
# 3) generate shared object file
make win
# 3) link your project to gccgc
gcc -o main.exe main.o -L /path/to/gccgc -l_gccgc.dll
# you may need to add the lib location to your PATH
```
