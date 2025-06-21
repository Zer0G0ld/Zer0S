CC = i686-elf-gcc
AS = nasm
LD = i686-elf-ld

SRC_DIR = src
OBJ_DIR = build/obj
BIN_DIR = build/bin
ISO_DIR = iso
ISO_NAME = build/zer0s.iso
KERNEL_BIN = $(BIN_DIR)/kernel.elf
ISR_STUB_ASM_SRC = src/isr_stub.asm
ISR_STUB_ASM_OBJ = $(OBJ_DIR)/isr_stub.o

CFLAGS = -ffreestanding -m32 -nostdlib -nostartfiles -Wall -Wextra -O2 -Iinclude -Isrc -Iterminal
ASFLAGS = -f elf32
LDFLAGS = -T linker.ld

GDT_ASM_SRC = src/gdt_flush.asm
GDT_ASM_OBJ = $(OBJ_DIR)/gdt_flush.o

SRC_FILES = $(SRC_DIR)/kernel.c $(SRC_DIR)/gdt.c $(SRC_DIR)/idt.c $(SRC_DIR)/isr.c $(SRC_DIR)/irq.c $(SRC_DIR)/interrupts.c $(SRC_DIR)/keyboard.c $(SRC_DIR)/screen.c
OBJ_FILES = $(addprefix $(OBJ_DIR)/, $(notdir $(SRC_FILES:.c=.o))) $(GDT_ASM_OBJ) $(ISR_STUB_ASM_OBJ)

# ========================
# Regras de Build
# ========================

build: $(OBJ_DIR) $(BIN_DIR) $(KERNEL_BIN)

$(OBJ_DIR):
	@echo "[+] Criando pasta de objetos: $@"
	mkdir -p $@

$(BIN_DIR):
	@echo "[+] Criando pasta de binários: $@"
	mkdir -p $@

$(ISO_DIR)/boot/grub:
	@echo "[+] Criando estrutura de diretórios ISO: $@"
	mkdir -p $(ISO_DIR)/boot/grub

$(ISR_STUB_ASM_OBJ): $(ISR_STUB_ASM_SRC) | $(OBJ_DIR)
	@echo "[+] Montando isr_stub.asm: $<"
	$(AS) $(ASFLAGS) $< -o $@

$(KERNEL_BIN): $(OBJ_FILES) | $(BIN_DIR)
	@echo "[+] Linkando Kernel..."
	$(LD) $(LDFLAGS) -o $@ $(OBJ_FILES)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "[+] Compilando C: $<"
	$(CC) $(CFLAGS) -c $< -o $@

$(GDT_ASM_OBJ): $(GDT_ASM_SRC) | $(OBJ_DIR)
	@echo "[+] Compilando Assembly: $<"
	$(AS) $(ASFLAGS) $< -o $@

# ========================
# Geração da ISO + Check automático
# ========================

iso: $(KERNEL_BIN) $(ISO_DIR)/boot/grub
	@echo "[+] Copiando Kernel para ISO..."
	cp $(KERNEL_BIN) $(ISO_DIR)/boot/kernel.elf
	cp grub/grub.cfg $(ISO_DIR)/boot/grub/
	@echo "[+] Gerando ISO com GRUB..."
	grub-mkrescue -o $(ISO_NAME) $(ISO_DIR)
	@$(MAKE) check

# ========================
# Checagem do ELF e Multiboot
# ========================

check:
	@echo "==========================================="
	@echo ">>> Verificando ELF Header e Multiboot <<<"
	@echo "==========================================="
	@echo "[*] Lendo ELF Header:"
	@readelf -h $(KERNEL_BIN) || echo "[ERRO] ELF Header inválido!"

	@echo ""
	@echo "[*] Seções do ELF:"
	@readelf -S $(KERNEL_BIN)

	@echo ""
	@echo "[*] Procurando Multiboot Header:"
	@readelf -S $(KERNEL_BIN) | grep .multiboot || echo "[ERRO] Multiboot Header NÃO encontrado!"

	@echo ""
	@echo "[*] Layout de Seções (objdump):"
	@objdump -h $(KERNEL_BIN) | grep -E "multiboot|text|data|bss|stack"

	@echo "==========================================="
	@echo ">>> Fim da checagem <<<"
	@echo "==========================================="

# ========================
# Clean + Run
# ========================

clean:
	@echo "[+] Limpando build..."
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(ISO_DIR) $(ISO_NAME)

run: iso
	@echo "[+] Rodando no QEMU..."
	qemu-system-i386 -cdrom $(ISO_NAME)
