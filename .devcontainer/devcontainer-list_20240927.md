Tools needed to dev and build WorkHorz (mandatory)

## Oct 16th Version

Ubuntu OS >= 24.04 (soon 24.10)
    GCC >= 14.1 (soon 14.2)
    Standard: C++23 (soon C++26)
    MoLD >= 2.30 (linker)
    Ninja >= 1.11.1 (builder)
    CMake >= 3.28
    CCache >= 4.9.1 (compiler cache)
    DOxygen >= 1.9.8 (documentation generator)
    vcpkg (newest)
    bash
    lua5.4 (package Ubuntu)
    cppCheck >= 2.13
    OpenSSL >= 3.0.13
    SQLite >= 3.44

Installing newer version of GCC on Ubunutu

    Explained here: https://www.dedicatedcore.com/blog/install-gcc-compiler-ubuntu/ 

Autoconf tools:
      autoconf >= 2.71
      automake >= 1.16.5
      autoconf-archive >= 20220903-3
Autotools
ICU:
  libicu74 >= 74.2
  icu-devtools
libtool (for libsodium at least)
PNG: libpng-dev
libzip
liblzma


##December 2025
-------------
Component,Previous Target,Dec 2025 Update,Installation Method
GDB,14.2,15.1 (Latest stable release),Build from Source
CMake,≥3.28,≥3.30.x (Latest in 24.04 repo),apt package
Ninja,≥1.11.1,≥1.12 (Latest in 24.04 repo),apt package
CCache,≥4.9.1,≥4.10 (Latest in 24.04 repo),apt package
Doxygen,≥1.9.8,≥1.11 (Latest in 24.04 repo),apt package
CPPCheck,≥2.13,≥2.15 (Latest in 24.04 repo),apt package
OpenSSL,≥3.0.13,≥3.2 (Latest in 24.04 repo),libssl-dev package
SQLite,≥3.44,≥3.45 (Latest in 24.04 repo),libsqlite3-dev package
Autoconf,≥2.71,≥2.72 (Latest in 24.04 repo),apt package
libicu,≥74.2,≥75.1 (Latest in 24.04 repo),libicu-dev package