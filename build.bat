@echo off

clang -target x86_64-unknown-windows -ffreestanding -nostdlib -isystem "C:\Program Files\LLVM\include" -c kernel.c -o kernel.o

lld-link /SUBSYSTEM:EFI_APPLICATION /ENTRY:efi_main kernel.o /OUT:bootx64.efi
pause