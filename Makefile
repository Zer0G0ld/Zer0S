# Definindo o compilador e as flags
CC = i686-elf-gcc
AS = nasm
LD = ld

# Diretórios
SRC_DIR = src
OBJ_DIR = build/obj
BIN_DIR = build/bin
ISO_DIR = iso
KERNEL_BIN = $(BIN_DIR)/kernel.bin
BOOT_BIN = $(BIN_DIR)/boot.bin
ISO_NAME = $(ISO_DIR)/zer0s.iso
GDT_ASM_SRC = src/gdt_flush.asm
GDT_ASM_OBJ = $(OBJ_DIR)/gdt_flush.o

# Arquivos de fonte
KERNEL_SRC = $(SRC_DIR)/kernel.c
BOOT_SRC = boot/boot.asm

# Adicione a definição do screen.c
SCREEN_SRC = $(SRC_DIR)/screen.c  # Adicione esta linha

# Flags de compilação
CFLAGS = -ffreestanding -m32 -mno-red-zone -fno-builtin -nostdlib -nostartfiles -Wall -Wextra -O2 -Iinclude -Isrc -Iterminal
ASFLAGS = -f elf

# Criação dos diretórios
$(OBJ_DIR):
	@echo "Criando diretório de objetos..."
	mkdir -p $@

$(BIN_DIR):
	@echo "Criando diretório binário..."
	mkdir -p $@

# Adicionar os novos arquivos de origem
GDT_SRC = $(SRC_DIR)/gdt.c
IDT_SRC = $(SRC_DIR)/idt.c
ISR_SRC = $(SRC_DIR)/isr.c
INTERRUPTS_SRC = $(SRC_DIR)/interrupts.c

# Adicionar a dependência do shell.o e outros objetos
KERNEL_OBJS = $(OBJ_DIR)/kernel.o $(OBJ_DIR)/gdt.o $(OBJ_DIR)/interrupts.o $(GDT_ASM_OBJ) $(OBJ_DIR)/shell.o $(OBJ_DIR)/keyboard.o $(OBJ_DIR)/utils.o $(OBJ_DIR)/idt.o $(OBJ_DIR)/isr.o $(OBJ_DIR)/screen.o

# Regras para compilar o código
all: build $(ISO_NAME)

build: $(OBJ_DIR) $(BIN_DIR) $(KERNEL_BIN) $(BOOT_BIN)

$(ISO_NAME): $(KERNEL_BIN) $(BOOT_BIN)
	@echo "Gerando a ISO..."
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(BOOT_BIN) $(ISO_DIR)/boot/
	cp $(KERNEL_BIN) $(ISO_DIR)/boot/
	cp ./grub/grub.cfg $(ISO_DIR)/boot/grub/
	grub-mkrescue -o $(ISO_NAME) $(ISO_DIR)

$(KERNEL_BIN): $(KERNEL_OBJS) | $(BIN_DIR)
	@echo "Linkando o Kernel..."
	$(LD) -melf_i386 -T linker.ld $(KERNEL_OBJS) -o $@

$(OBJ_DIR)/kernel.o: $(KERNEL_SRC) | $(OBJ_DIR)
	@echo "Compilando o Kernel a partir de $(KERNEL_SRC)..."
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/gdt.o: $(GDT_SRC) | $(OBJ_DIR)
	@echo "Compilando o GDT a partir de $(GDT_SRC)..."
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/interrupts.o: $(INTERRUPTS_SRC) | $(OBJ_DIR)
	@echo "Compilando o arquivo de interrupções a partir de $(INTERRUPTS_SRC)..."
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/idt.o: $(IDT_SRC) | $(OBJ_DIR)
	@echo "Compilando o IDT a partir de $(IDT_SRC)..."
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/isr.o: $(ISR_SRC) | $(OBJ_DIR)
	@echo "Compilando o ISR a partir de $(ISR_SRC)..."
	@$(CC) $(CFLAGS) -c $< -o $@

$(BOOT_BIN): $(BOOT_SRC) | $(BIN_DIR)
	@echo "Compilando o Bootloader a partir de $(BOOT_SRC)..."
	@$(AS) $(ASFLAGS) $< -o $@

$(GDT_ASM_OBJ): $(GDT_ASM_SRC)
	@echo "Compilando o GDT Assembly a partir de $(GDT_ASM_SRC)..."
	nasm -f elf32 $< -o $@

$(OBJ_DIR)/keyboard.o: src/keyboard.c include/keyboard.h | $(OBJ_DIR)
	@echo "Compilando o Keyboard a partir de src/keyboard.c..."
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/utils.o: terminal/utils.c include/utils.h | $(OBJ_DIR)
	@echo "Compilando utils a partir de terminal/utils.c..."
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/shell.o: terminal/shell.c include/shell.h | $(OBJ_DIR)
	@echo "Compilando o Shell a partir de terminal/shell.c..."
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/screen.o: $(SCREEN_SRC) | $(OBJ_DIR)
	@echo "Compilando o Screen a partir de $(SCREEN_SRC)..."
	@test -f $(SCREEN_SRC) || { echo "Erro: $(SCREEN_SRC) não encontrado!"; exit 1; }
	$(CC) $(CFLAGS) -c $(SCREEN_SRC) -o $(OBJ_DIR)/screen.o

clean:
	@echo "Limpando os arquivos de compilação..."
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(ISO_DIR) $(KERNEL_BIN) $(BOOT_BIN)

run: $(ISO_NAME)
	@echo "Iniciando o sistema no QEMU..."
	qemu-system-i386 -cdrom $(ISO_NAME)
