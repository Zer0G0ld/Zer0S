#!/bin/bash

set -e

# Zer0S - GCC Cross Compiler Setup Script
# Por: Zer0 😎

TARGET=i686-elf
PREFIX=/usr/local/cross
NPROC=$(nproc)
SRC_DIR=~/cross-tools
PATH=$PATH:$PREFIX/bin

echo "========================================"
echo " Zer0S - Configuração do Cross Compiler "
echo " Target: $TARGET"
echo " Prefix: $PREFIX"
echo " CPUs disponíveis: $NPROC"
echo "========================================"

# Verificar se já existe o cross-compiler
if command -v $TARGET-gcc > /dev/null 2>&1; then
    echo "[✔] Cross-compiler '$TARGET-gcc' já está instalado!"
    $TARGET-gcc --version
    exit 0
fi

# Instalar dependências básicas
echo "[1/5] Instalando dependências..."
sudo apt update
sudo apt install -y build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo xorriso grub-pc-bin qemu-system-x86 nasm wget

# Criar diretório de fontes
mkdir -p $SRC_DIR
cd $SRC_DIR

# Baixar e construir Binutils
echo "[2/5] Baixando e compilando Binutils..."
if [ ! -f binutils-2.40.tar.gz ]; then
    wget https://ftp.gnu.org/gnu/binutils/binutils-2.40.tar.gz
fi
tar -xzf binutils-2.40.tar.gz
mkdir -p build-binutils
cd build-binutils
../binutils-2.40/configure --target=$TARGET --prefix=$PREFIX --with-sysroot --disable-nls --disable-werror
make -j$NPROC
sudo make install
cd ..

# Baixar e construir GCC (somente C)
echo "[3/5] Baixando e compilando GCC..."
if [ ! -f gcc-13.1.0.tar.gz ]; then
    wget https://ftp.gnu.org/gnu/gcc/gcc-13.1.0/gcc-13.1.0.tar.gz
fi
tar -xzf gcc-13.1.0.tar.gz
mkdir -p build-gcc
cd build-gcc
../gcc-13.1.0/configure --target=$TARGET --prefix=$PREFIX --disable-nls --enable-languages=c --without-headers
make all-gcc -j$NPROC
make all-target-libgcc -j$NPROC
sudo make install-gcc install-target-libgcc
cd ..

# Garantir PATH no bashrc apenas uma vez
if ! grep -q "$PREFIX/bin" ~/.bashrc; then
    echo "export PATH=\$PATH:$PREFIX/bin" >> ~/.bashrc
fi
export PATH=$PATH:$PREFIX/bin

echo "========================================"
echo " Cross Compiler instalado com sucesso!"
echo " Teste com: $TARGET-gcc --version"
echo "========================================"
