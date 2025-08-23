# MODDN
Moddn is a universal Lua-JIT mod loader written in C.

## Build
### Windows (32-bit)
Requires [MSYS2](https://www.msys2.org/) (MinGW32) installed

Install GCC and Make
```
pacman -S mingw-w64-i686-gcc mingw-w64-i686-make git
```
download Moddn
```
git clone --recurse-submodules https://github.com/Milkmen/Moddn.git
```
build Lua-JIT and Moddn
```
cd Moddn/lib/luajit/src
mingw32-make
cd ../../..
mingw32-make
```