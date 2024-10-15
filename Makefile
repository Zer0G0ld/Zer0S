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
CFLAGS = -ffreestanding -m32 -mno-red-zone -fno-builtin -Wall -Wextra -O2 -Iinclude -Isrc -Iterminal
ASFLAGS = -f elf

# Adicionar os novos arquivos de origem
GDT_SRC = $(SRC_DIR)/gdt.c
IDT_SRC = $(SRC_DIR)/idt.c
ISR_SRC = $(SRC_DIR)/isr.c
INTERRUPTS_SRC = $(SRC_DIR)/interrupts.c

# Adicionar a dependência do shell.o e outros objetos
KERNEL_OBJS = $(OBJ_DIR)/kernel.o $(OBJ_DIR)/gdt.o $(OBJ_DIR)/interrupts.o $(GDT_ASM_OBJ) $(OBJ_DIR)/shell.o $(OBJ_DIR)/keyboard.o $(OBJ_DIR)/utils.o $(OBJ_DIR)/idt.o $(OBJ_DIR)/isr.o

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

$(OBJ_DIR)/idt.o: $(IDT_SRC)
	@echo "Compilando o IDT..."
	$(CC) $(CFLAGS) -c $(IDT_SRC) -o $(OBJ_DIR)/idt.o

$(OBJ_DIR)/isr.o: $(ISR_SRC)
	@echo "Compilando o ISR..."
	$(CC) $(CFLAGS) -c $(ISR_SRC) -o $(OBJ_DIR)/isr.o

$(BOOT_BIN): $(BOOT_SRC)
	@echo "Compilando o Bootloader..."
	$(AS) $(BOOT_SRC) -o $(BOOT_BIN)

$(GDT_ASM_OBJ): $(GDT_ASM_SRC)
	@echo "Compilando o GDT Assembly..."
	nasm -f elf32 $(GDT_ASM_SRC) -o $(GDT_ASM_OBJ)

$(OBJ_DIR)/keyboard.o: src/keyboard.c include/keyboard.h
	@echo "Compilando o Keyboard..."
	$(CC) $(CFLAGS) -c src/keyboard.c -o $(OBJ_DIR)/keyboard.o

# Caminho correto para utils
$(OBJ_DIR)/utils.o: terminal/utils.c include/utils.h
	@echo "Compilando utils..."
	$(CC) $(CFLAGS) -c terminal/utils.c -o $(OBJ_DIR)/utils.o

$(OBJ_DIR)/shell.o: terminal/shell.c include/shell.h
	@echo "Compilando o Shell..."
	$(CC) $(CFLAGS) -c terminal/shell.c -o $(OBJ_DIR)/shell.o

clean:
	@echo "Limpando os arquivos de compilação..."
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(ISO_DIR) $(KERNEL_BIN) $(BOOT_BIN)

run: $(ISO_NAME)
	@echo "Iniciando o sistema no QEMU..."
	qemu-system-x86_64 -cdrom $(ISO_NAME)

# Cria o diretório build, se ele não existir
build:
	mkdir -p build
