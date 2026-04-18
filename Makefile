# Makefile for Zer0S 64-bit Kernel

PREFIX := $(HOME)/opt/cross
CC := $(PREFIX)/bin/x86_64-elf-gcc
AS := nasm
LD := $(PREFIX)/bin/x86_64-elf-ld

CFLAGS := -ffreestanding -nostdlib -nostdinc -O2 -Wall -Wextra \
          -Ikernel/include -mno-red-zone -mgeneral-regs-only \
          -mcmodel=kernel -fno-stack-protector

LDFLAGS := -n -T linker.ld

SRC_DIR := kernel
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj

# Source files
ASM_SOURCES := kernel/arch/x86_64/boot.asm
C_SOURCES := kernel/kernel_main.c

# Object files
ASM_OBJECTS := $(OBJ_DIR)/boot.o
C_OBJECTS := $(OBJ_DIR)/kernel_main.o
ALL_OBJECTS := $(ASM_OBJECTS) $(C_OBJECTS)

# Build rules
$(OBJ_DIR)/boot.o: kernel/arch/x86_64/boot.asm
	@mkdir -p $(dir $@)
	$(AS) -f elf64 $< -o $@

$(OBJ_DIR)/kernel_main.o: kernel/kernel_main.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all clean iso run debug

all: kernel

kernel: $(ALL_OBJECTS)
	@mkdir -p $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $(BUILD_DIR)/zer0s.elf $^

iso: kernel
	@mkdir -p iso/boot/grub
	cp $(BUILD_DIR)/zer0s.elf iso/boot/
	cp grub.cfg iso/boot/grub/
	grub-mkrescue -o $(BUILD_DIR)/zer0s.iso iso/

run: iso
	qemu-system-x86_64 -cdrom $(BUILD_DIR)/zer0s.iso -serial stdio -m 2G -cpu max

debug: iso
	qemu-system-x86_64 -cdrom $(BUILD_DIR)/zer0s.iso -s -S -serial stdio -cpu max &
	sleep 1
	gdb $(BUILD_DIR)/zer0s.elf -ex "target remote localhost:1234" -ex "break kernel_main" -ex "continue"

clean:
	rm -rf $(BUILD_DIR) iso
