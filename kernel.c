/* =============================================================
 * kernel.c - Kernel principal (v0.4)
 * =============================================================
 * Ahora con MINI-SHELL. Inicializa todo el sistema y entrega el
 * control a la shell, que interpreta los comandos que escribis.
 * ============================================================= */
#include "vga.h"
#include "gdt.h"
#include "idt.h"
#include "irq.h"
#include "timer.h"
#include "keyboard.h"
#include "shell.h"
#include "fs.h"

#if defined(__linux__)
#error "Estas usando tu gcc de Linux. Necesitas un cross-compiler i686-elf."
#endif
#if !defined(__i386__)
#error "Este kernel debe compilarse con un compilador ix86-elf."
#endif

void kernel_main(void) {
    terminal_initialize();

    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    terminal_writestring("===========================================\n");
    terminal_writestring("   miSO v0.5 - Con sistema de archivos!\n");
    terminal_writestring("===========================================\n\n");

    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));

    gdt_install();
    terminal_writestring("[OK] GDT cargada.\n");

    idt_install();
    terminal_writestring("[OK] IDT cargada.\n");

    irq_install();
    terminal_writestring("[OK] IRQs configuradas (PIC remapeado).\n");

    timer_install(100);
    terminal_writestring("[OK] Timer instalado a 100 Hz.\n");

    keyboard_install();
    terminal_writestring("[OK] Teclado instalado.\n");

    fs_init();
    terminal_writestring("[OK] Sistema de archivos en RAM listo.\n\n");

    /* Habilitar interrupciones. */
    asm volatile ("sti");

    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    terminal_writestring("Bienvenido! Escribi 'help' para empezar.\n\n");

    /* Arrancar la shell (muestra el primer prompt). */
    shell_init();

    /* Loop principal: dormir hasta la proxima interrupcion (tecla/timer).
     * Toda la accion pasa ahora en los manejadores de interrupcion. */
    for (;;) {
        asm volatile ("hlt");
    }
}
