CC = clang
LD = ld.lld
AS = nasm

ARCH = x86_64-elf
BUILD_DIR = target
BOOT_DIR = boot
KERNEL_NAME = kernel.elf

CFLAGS = -target $(ARCH) -nostdlib -std=c23 -m64\
-mcmodel=kernel -Isrc/include -O0 -Wall -Wextra -Werror\
-ffreestanding \
    -fno-stack-protector \
    -fno-stack-check \
    -fno-PIC \
    -ffunction-sections \
    -fdata-sections \
    -m64 \
		-g -c \
    -march=x86-64 \
		-mno-80387 \
    -mno-mmx \
    -mno-red-zone \
    -mcmodel=kernel


ASFLAGS := -f elf64 -g

LDFLAGS = -T $(BOOT_DIR)/linker.ld -nostdlib --gc-sections

KERNEL_SRCS := $(shell find src/kern -name '*.c' -o -name '*.S')
LIB_SRCS := $(shell find src/libs -name '*.c' -o -name '*.S')
USERSPACE_SRCS := $(shell find src/userspace -name '*.c' -o -name '*.S')

ALL_SRCS = $(KERNEL_SRCS) $(LIB_SRCS) $(USERSPACE_SRCS)
OBJS = $(patsubst src/%,$(BUILD_DIR)/%,$(ALL_SRCS:.c=.o)) $(FONT_OBJ)
OBJS := $(OBJS:.S=.o)

FONT = src/kern/fonts/output.psf
FONT_OBJ = $(BUILD_DIR)/kern/fonts/font.o

PATH_TOOLS := $(abspath tools/bin)
LIMINE_DIR := $(abspath tools/limine)
export PATH := $(PATH_TOOLS):$(PATH)

ISO_NAME = OS64.iso

.PHONY: all clean run

all: $(ISO_NAME)

dev: limine-setup clean all run-iso

$(BUILD_DIR)/$(KERNEL_NAME): $(OBJS)
	@echo "Linking kernel..."
	@$(LD) $(LDFLAGS) -o $@ $^
	@echo "Kernel built: $@"

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: src/%.S
	@mkdir -p $(dir $@)
	@echo "NASM  $<"
	@$(AS) $(ASFLAGS) $< -o $@

$(FONT_OBJ): $(FONT)
	@mkdir -p $(dir $@)
	@echo "OBJCOPY $<"
	@objcopy -O elf64-x86-64 -B i386 -I binary $< $@
	@objcopy \
		--redefine-sym _binary_$(subst /,_,$(subst .,_,$(subst -,_,$(subst _,_,$<))))_start=_psf2_font_start \
		--redefine-sym _binary_$(subst /,_,$(subst .,_,$(subst -,_,$(subst _,_,$<))))_end=_psf2_font_end \
		--redefine-sym _binary_$(subst /,_,$(subst .,_,$(subst -,_,$(subst _,_,$<))))_size=_psf2_font_size \
		$@

$(ISO_NAME): $(BUILD_DIR)/$(KERNEL_NAME) $(BOOT_DIR)/limine.conf
	@echo "Building ISO image..."
	@rm -rf $(BUILD_DIR)/iso_root
	@mkdir -p $(BUILD_DIR)/iso_root/boot/limine
	
	@cp -v $(BUILD_DIR)/$(KERNEL_NAME) $(BUILD_DIR)/iso_root/boot/kernel.elf
	@cp -v $(BOOT_DIR)/limine.conf $(BUILD_DIR)/iso_root/boot/limine/
	
	@cp -v $(LIMINE_DIR)/limine-bios.sys \
		$(LIMINE_DIR)/limine-bios-cd.bin \
		$(LIMINE_DIR)/limine-uefi-cd.bin \
		$(BUILD_DIR)/iso_root/boot/limine/
	
	@mkdir -p $(BUILD_DIR)/iso_root/EFI/BOOT
	@cp -v $(LIMINE_DIR)/BOOTX64.EFI $(BUILD_DIR)/iso_root/EFI/BOOT/
	@cp -v $(LIMINE_DIR)/BOOTAA64.EFI $(BUILD_DIR)/iso_root/EFI/BOOT/
	
	@xorriso -as mkisofs \
		-R -r -J -b boot/limine/limine-bios-cd.bin \
		-iso-level 3 \
		-partition_offset 16 \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot boot/limine/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image \
		--protective-msdos-label \
		-o $(ISO_NAME) $(BUILD_DIR)/iso_root

	limine bios-install $(ISO_NAME)

	@rm -rf $(BUILD_DIR)/iso_root
	@echo "ISO created: $(ISO_NAME)"

run-iso: $(ISO_NAME)
	@echo "Starting QEMU..."
	@qemu-system-x86_64 -cdrom $(ISO_NAME) -m 2G -serial stdio -machine q35 -cpu qemu64

limine-setup:
	@./tools/limine.sh

clean:
	@rm -rf $(BUILD_DIR)

dist-clean: clean
	@rm -f $(ISO_NAME)
	@rm -rf tools/bin tools/limine

$(shell mkdir -p $(BUILD_DIR))
