/* =============================================================
 * vga.c - Implementacion de la pantalla en modo texto
 * =============================================================
 * Igual que el kernel.c original, pero ahora con SCROLL de verdad:
 * cuando se llena la pantalla, sube todo una linea en vez de volver
 * al tope. Esto es necesario ahora que vamos a escribir mucho
 * (cada tecla que apretes).
 * ============================================================= */
#include "vga.h"
#include "string.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_buffer = (uint16_t*) 0xB8000;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[y * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_setcolor(uint8_t color) {
    terminal_color = color;
}

static void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    terminal_buffer[y * VGA_WIDTH + x] = vga_entry(c, color);
}

/* Sube todo el contenido una linea (scroll). La ultima linea queda
 * en blanco, lista para escribir. */
static void terminal_scroll(void) {
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[(y - 1) * VGA_WIDTH + x] =
                terminal_buffer[y * VGA_WIDTH + x];
        }
    }
    /* Limpiar la ultima linea. */
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] =
            vga_entry(' ', terminal_color);
    }
}

void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_scroll();
            terminal_row = VGA_HEIGHT - 1;
        }
        return;
    }

    /* Backspace: retrocede el cursor y borra el caracter. */
    if (c == '\b') {
        if (terminal_column > 0) {
            terminal_column--;
            terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
        }
        return;
    }

    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_scroll();
            terminal_row = VGA_HEIGHT - 1;
        }
    }
}

void terminal_writestring(const char* data) {
    size_t len = strlen(data);
    for (size_t i = 0; i < len; i++)
        terminal_putchar(data[i]);
}

/* Imprime un numero entero sin signo en decimal. Como no tenemos
 * printf, convertimos el numero a sus digitos a mano. */
void terminal_writedec(uint32_t n) {
    if (n == 0) {
        terminal_putchar('0');
        return;
    }
    char buf[11];          /* un uint32 tiene como mucho 10 digitos */
    int i = 0;
    while (n > 0) {
        buf[i++] = '0' + (n % 10);  /* sacar el ultimo digito */
        n /= 10;
    }
    /* Los digitos quedaron al reves; imprimirlos de atras para adelante. */
    while (i > 0) {
        terminal_putchar(buf[--i]);
    }
}
