
# Makefile for DPOSBoot with LLVM/Clang 22
# Windows UEFI development

CC = clang
CFLAGS = -target x86_64-unknown-windows -ffreestanding -fshort-wchar -mno-red-zone
CFLAGS += -O2 -Wall -Wextra -nostdlib -fno-stack-protector -fno-builtin
LDFLAGS = -nostdlib -Wl,-dll -Wl,--subsystem,10 -Wl,-e,efi_main
LDFLAGS += -Wl,--image-base,0x200000000

TARGET = bootx64.efi
SOURCES = kernel.c

all: $(TARGET)
	@echo ✅ Build complete: $(TARGET)
	@dir $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o $@
	@echo 📦 Created EFI application

clean:
	-del /Q *.efi 2>nul
	@echo 🧹 Cleaned up

# Direct compile command (alternative)
simple:
	clang -target x86_64-unknown-windows -ffreestanding -nostdlib ^
	      -Wl,-dll -Wl,--subsystem,10 -Wl,-e,efi_main ^
	      kernel.c -o bootx64.efi
	@echo ✅ Built with simple command

# Create bootable ISO (requires xorriso)
iso: $(TARGET)
	-if not exist iso_root mkdir iso_root
	-if not exist iso_root\EFI mkdir iso_root\EFI
	-if not exist iso_root\EFI\BOOT mkdir iso_root\EFI\BOOT
	copy /Y $(TARGET) iso_root\EFI\BOOT\ >nul
	@echo 📀 Creating ISO...
	@if exist "C:\Program Files\xorriso\xorriso.exe" ( \
		"C:\Program Files\xorriso\xorriso.exe" -as mkisofs -o dposboot.iso ^
		-eltorito-alt-boot -e /EFI/BOOT/$(TARGET) -no-emul-boot iso_root/ ^
	) else ( \
		echo ⚠️ xorriso not found - ISO not created ^
	)

.PHONY: all clean simple iso