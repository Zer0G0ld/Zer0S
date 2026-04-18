#!/bin/bash
set -e

export PREFIX="$HOME/opt/cross"
export TARGET=x86_64-elf
export PATH="$PREFIX/bin:$PATH"

cd ~/src

# Versões que existem no FTP da GNU
BINUTILS_VERSION="2.41"
GCC_VERSION="13.2.0"

# Download dos arquivos
if [ ! -f binutils-$BINUTILS_VERSION.tar.xz ]; then
    wget https://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VERSION.tar.xz
fi

if [ ! -f gcc-$GCC_VERSION.tar.xz ]; then
    wget https://ftp.gnu.org/gnu/gcc/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.xz
fi

# Binutils
tar -xf binutils-$BINUTILS_VERSION.tar.xz
mkdir -p build-binutils
cd build-binutils
../binutils-$BINUTILS_VERSION/configure --target=$TARGET --prefix="$PREFIX" \
    --with-sysroot --disable-nls --disable-werror
make -j$(nproc)
make install
cd ..

# GCC
tar -xf gcc-$GCC_VERSION.tar.xz
mkdir -p build-gcc
cd build-gcc
../gcc-$GCC_VERSION/configure --target=$TARGET --prefix="$PREFIX" \
    --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc -j$(nproc)
make all-target-libgcc -j$(nproc)
make install-gcc
make install-target-libgcc
cd ..

echo "✅ Cross-compiler installed at $PREFIX"
echo "Add to ~/.bashrc: export PATH=\"$PREFIX/bin:\$PATH\""
