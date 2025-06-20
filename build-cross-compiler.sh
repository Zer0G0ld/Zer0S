#!/bin/bash

set -e

# Zer0S - GCC Cross Compiler Setup Script
# Por: Zer0 😎

TARGET=i686-elf
PREFIX=/usr/local/cross
NPROC=$(nproc)
SRC_DIR=~/cross-tools
PATH=$PATH:$PREFIX/bin

echo "==============================="
echo " Iniciando instalação do Cross Compiler"
echo " Target: $TARGET"
echo " Prefix: $PREFIX"
echo " CPUs disponíveis: $NPROC"
echo "==============================="

# Instalar dependências básicas
echo "[1/5] Instalando dependências via apt..."
sudo apt update
sudo apt install -y build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo xorriso grub-pc-bin qemu-system-x86 nasm wget

# Criar diretório de fontes
mkdir -p $SRC_DIR
cd $SRC_DIR

# Baixar Binutils
echo "[2/5] Baixando Binutils..."
if [ ! -f binutils-2.40.tar.gz ]; then
  wget https://ftp.gnu.org/gnu/binutils/binutils-2.40.tar.gz
fi
tar -xzf binutils-2.40.tar.gz

# Build Binutils
echo "[3/5] Compilando Binutils..."
mkdir -p build-binutils
cd build-binutils
../binutils-2.40/configure --target=$TARGET --prefix=$PREFIX --with-sysroot --disable-nls --disable-werror
make -j$NPROC
sudo make install
cd ..

# Baixar GCC
echo "[4/5] Baixando GCC..."
if [ ! -f gcc-13.1.0.tar.gz ]; then
  wget https://ftp.gnu.org/gnu/gcc/gcc-13.1.0/gcc-13.1.0.tar.gz
fi
tar -xzf gcc-13.1.0.tar.gz

# Build GCC
echo "[5/5] Compilando GCC..."
mkdir -p build-gcc
cd build-gcc
../gcc-13.1.0/configure --target=$TARGET --prefix=$PREFIX --disable-nls --enable-languages=c --without-headers
make all-gcc -j$NPROC
make all-target-libgcc -j$NPROC
sudo make install

# Adicionar PATH ao bashrc
echo "export PATH=\$PATH:$PREFIX/bin" >> ~/.bashrc
source ~/.bashrc

echo "==============================="
echo " Cross Compiler instalado com sucesso!"
echo " Teste com: i686-elf-gcc --version"
echo "==============================="
