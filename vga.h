/* =============================================================
 * vga.h - Escritura en pantalla (modo texto VGA)
 * =============================================================
 * Sacamos las funciones de pantalla del kernel.c original a su propio
 * modulo, asi el codigo queda ordenado. Mismo principio: escribir en
 * la memoria 0xB8000 hace aparecer caracteres en pantalla.
 * ============================================================= */
#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include <stddef.h>

enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};

void terminal_initialize(void);
void terminal_setcolor(uint8_t color);
void terminal_putchar(char c);
void terminal_writestring(const char* data);
void terminal_writedec(uint32_t n);   /* imprime un numero entero en decimal */
uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg);

#endif
