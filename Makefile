CC  = i686-elf-gcc
AS  = i686-elf-as

CFLAGS  = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
LDFLAGS = -ffreestanding -O2 -nostdlib

# Todos los objetos que componen el kernel.
OBJS = boot.o gdt_flush.o idt_load.o irq_asm.o \
       gdt.o idt.o irq.o vga.o timer.o keyboard.o string.o fs.o shell.o kernel.o

all: myos.bin

# --- Archivos Assembly ---
boot.o: boot.s
	$(AS) boot.s -o boot.o

gdt_flush.o: gdt_flush.s
	$(AS) gdt_flush.s -o gdt_flush.o

idt_load.o: idt_load.s
	$(AS) idt_load.s -o idt_load.o

irq_asm.o: irq_asm.s
	$(AS) irq_asm.s -o irq_asm.o

# --- Archivos C ---
gdt.o: gdt.c
	$(CC) -c gdt.c -o gdt.o $(CFLAGS)

idt.o: idt.c
	$(CC) -c idt.c -o idt.o $(CFLAGS)

irq.o: irq.c
	$(CC) -c irq.c -o irq.o $(CFLAGS)

vga.o: vga.c
	$(CC) -c vga.c -o vga.o $(CFLAGS)

timer.o: timer.c
	$(CC) -c timer.c -o timer.o $(CFLAGS)

keyboard.o: keyboard.c
	$(CC) -c keyboard.c -o keyboard.o $(CFLAGS)

string.o: string.c
	$(CC) -c string.c -o string.o $(CFLAGS)

fs.o: fs.c
	$(CC) -c fs.c -o fs.o $(CFLAGS)

shell.o: shell.c
	$(CC) -c shell.c -o shell.o $(CFLAGS)

kernel.o: kernel.c
	$(CC) -c kernel.c -o kernel.o $(CFLAGS)

# --- Enlazado ---
myos.bin: $(OBJS) linker.ld
	$(CC) -T linker.ld -o myos.bin $(LDFLAGS) $(OBJS) -lgcc

# --- ISO y arranque ---
iso: myos.bin
	mkdir -p isodir/boot/grub
	cp myos.bin isodir/boot/myos.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o myos.iso isodir

run: iso
	qemu-system-i386 -cdrom myos.iso

clean:
	rm -rf *.o myos.bin myos.iso isodir

.PHONY: all iso run clean
