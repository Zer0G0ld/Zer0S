# Makefile for Zer0S 64-bit Kernel

PREFIX := $(HOME)/opt/cross
CC := $(PREFIX)/bin/x86_64-elf-gcc
AS := nasm
LD := $(PREFIX)/bin/x86_64-elf-ld

CFLAGS := -ffreestanding -nostdlib -nostdinc -O2 -Wall -Wextra \
          -Ikernel/include \
          -Ikernel/arch/x86_64 \
          -mno-red-zone -mgeneral-regs-only \
          -mcmodel=large -fno-stack-protector

LDFLAGS := -n -T linker.ld

BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj

# All C files in kernel/arch/x86_64/
C_FILES := $(wildcard kernel/arch/x86_64/*.c)
C_OBJS := $(patsubst kernel/arch/x86_64/%.c, $(OBJ_DIR)/%.o, $(C_FILES))

# Assembly files
ASM_FILES := kernel/arch/x86_64/boot.asm \
             kernel/arch/x86_64/isr.asm \
             kernel/arch/x86_64/isr_default.asm
ASM_OBJS := $(patsubst kernel/arch/x86_64/%.asm, $(OBJ_DIR)/%.o, $(ASM_FILES))

# Kernel main
KERNEL_MAIN := $(OBJ_DIR)/kernel_main.o

# All objects
OBJS := $(ASM_OBJS) $(C_OBJS) $(KERNEL_MAIN)

# Rules
$(OBJ_DIR)/boot.o: kernel/arch/x86_64/boot.asm
	@mkdir -p $(OBJ_DIR)
	nasm -f elf64 $< -o $@

$(OBJ_DIR)/isr.o: kernel/arch/x86_64/isr.asm
	@mkdir -p $(OBJ_DIR)
	nasm -f elf64 $< -o $@

$(OBJ_DIR)/isr_default.o: kernel/arch/x86_64/isr_default.asm
	@mkdir -p $(OBJ_DIR)
	nasm -f elf64 $< -o $@

$(OBJ_DIR)/%.o: kernel/arch/x86_64/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/kernel_main.o: kernel/kernel_main.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all clean iso run

all: kernel

kernel: $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $(BUILD_DIR)/zer0s.elf $^

iso: kernel
	@mkdir -p iso/boot/grub
	cp $(BUILD_DIR)/zer0s.elf iso/boot/
	cp grub.cfg iso/boot/grub/
	grub-mkrescue -o $(BUILD_DIR)/zer0s.iso iso/

run: iso
	qemu-system-x86_64 -cdrom $(BUILD_DIR)/zer0s.iso -serial stdio -m 2G -cpu max

clean:
	rm -rf $(BUILD_DIR) iso
