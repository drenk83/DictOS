#!/bin/bash
cd /home/drenk83/Documents/OS1
yasm -f bin -o bootsect.bin bootsect.asm
gcc -fno-pie -ffreestanding -m32 -o kernel.o -c kernel.cpp
ld --oformat binary -Ttext 0x10000 -o kernel.bin --entry=kmain -m elf_i386 kernel.o
qemu -fda bootsect.bin -fdb kernel.bin
