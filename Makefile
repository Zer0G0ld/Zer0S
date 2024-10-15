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
GDT_ASM_SRC = src/gdt_flush.asm
GDT_ASM_OBJ = build/gdt_flush.o

# Arquivos de fonte
KERNEL_SRC = $(SRC_DIR)/kernel.c
BOOT_SRC = boot/boot.asm

# Flags de compilação
CFLAGS = -ffreestanding -mno-red-zone -fno-builtin -Wall -Wextra -O2 -Iinclude
ASFLAGS = -f elf

# Adicionar os novos arquivos de origem
GDT_SRC = $(SRC_DIR)/gdt.c
INTERRUPTS_SRC = $(SRC_DIR)/interrupts.c

# Adicionar as novas dependências
KERNEL_OBJS = $(OBJ_DIR)/kernel.o $(OBJ_DIR)/gdt.o $(OBJ_DIR)/interrupts.o $(GDT_ASM_OBJ)

# Regras para compilar o código
all: build $(ISO_NAME)

$(ISO_NAME): $(KERNEL_BIN) $(BOOT_BIN)
	@echo "Gerando a ISO..."
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(BOOT_BIN) $(ISO_DIR)/boot/
	cp $(KERNEL_BIN) $(ISO_DIR)/boot/
	cp ./grub/grub.cfg $(ISO_DIR)/boot/grub/
	grub-mkrescue -o $(ISO_NAME) $(ISO_DIR)

$(KERNEL_BIN): $(KERNEL_OBJS)
	@echo "Linkando o Kernel..."
	$(LD) -melf_i386 -T linker.ld $(KERNEL_OBJS) -o $(KERNEL_BIN)

$(OBJ_DIR)/kernel.o: $(KERNEL_SRC)
	@echo "Compilando o Kernel..."
	$(CC) $(CFLAGS) -c $(KERNEL_SRC) -o $(OBJ_DIR)/kernel.o

$(OBJ_DIR)/gdt.o: $(GDT_SRC)
	@echo "Compilando o GDT..."
	$(CC) $(CFLAGS) -c $(GDT_SRC) -o $(OBJ_DIR)/gdt.o

$(OBJ_DIR)/interrupts.o: $(INTERRUPTS_SRC)
	@echo "Compilando o arquivo de interrupções..."
	$(CC) $(CFLAGS) -c $(INTERRUPTS_SRC) -o $(OBJ_DIR)/interrupts.o

$(BOOT_BIN): $(BOOT_SRC)
	@echo "Compilando o Bootloader..."
	$(AS) $(BOOT_SRC) -o $(BOOT_BIN)

$(GDT_ASM_OBJ): $(GDT_ASM_SRC)
	@echo "Compilando o GDT Assembly..."
	nasm -f elf32 $(GDT_ASM_SRC) -o $(GDT_ASM_OBJ)

clean:
	@echo "Limpando os arquivos de compilação..."
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(ISO_DIR)

run: $(ISO_NAME)
	@echo "Iniciando o sistema no QEMU..."
	qemu-system-x86_64 -cdrom $(ISO_NAME)

# Cria o diretório build, se ele não existir
build:
	mkdir -p build
