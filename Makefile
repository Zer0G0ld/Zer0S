# Definindo o compilador e as flags
CC = gcc
AS = nasm
LD = ld

# Diretórios
SRC_DIR = src
OBJ_DIR = build
BIN_DIR = build
ISO_DIR = iso
KERNEL_BIN = build/kernel.bin
BOOT_BIN = build/boot.bin
ISO_NAME = build/zer0s.iso

# Arquivos de fonte
KERNEL_SRC = $(SRC_DIR)/kernel.c
BOOT_SRC = boot/boot.asm

# Flags de compilação
CFLAGS = -ffreestanding -mno-red-zone -fno-builtin -Wall -Wextra -O2 -Iinclude
ASFLAGS = -f elf

# Regras para compilar o código
all: $(ISO_NAME)

$(ISO_NAME): $(KERNEL_BIN) $(BOOT_BIN)
	@echo "Gerando a ISO..."
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(BOOT_BIN) $(ISO_DIR)/boot/
	cp $(KERNEL_BIN) $(ISO_DIR)/boot/
	cp ./grub/grub.cfg $(ISO_DIR)/boot/grub/
	grub-mkrescue -o $(ISO_NAME) $(ISO_DIR)

$(KERNEL_BIN): build $(KERNEL_SRC)
	@echo "Compilando o Kernel..."
	$(CC) $(CFLAGS) -c $(KERNEL_SRC) -o $(OBJ_DIR)/kernel.o
	$(LD) -T linker.ld $(OBJ_DIR)/kernel.o -o $(KERNEL_BIN)

$(BOOT_BIN): $(BOOT_SRC)
	@echo "Compilando o Bootloader..."
	$(AS) $(BOOT_SRC) -o $(BOOT_BIN)

clean:
	@echo "Limpando os arquivos de compilação..."
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(ISO_DIR)

run: $(ISO_NAME)
	@echo "Iniciando o sistema no QEMU..."
	qemu-system-x86_64 -cdrom $(ISO_NAME)

# Cria o diretório build, se ele não existir
build:
	mkdir -p build
