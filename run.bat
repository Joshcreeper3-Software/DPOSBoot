@echo off
echo Preparing boot directory...

:: Create directory structure
if not exist hda-contents\EFI\BOOT mkdir hda-contents\EFI\BOOT

:: Copy EFI file
copy bootx64.efi hda-contents\EFI\BOOT\BOOTX64.EFI /y

:: Check files
if not exist OVMF.fd (
    echo ERROR: OVMF.fd not found!
    pause
    exit /b 1
)

if not exist bootx64.efi (
    echo ERROR: bootx64.efi not found! Run make first.
    pause
    exit /b 1
)

:: Launch QEMU
echo Launching QEMU...
"C:\Program Files\qemu\qemu-system-x86_64.exe" ^
    -drive file=fat:rw:hda-contents,format=raw ^
    -pflash OVMF.fd ^
    -m 256 ^
    -vga std

pause