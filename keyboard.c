/* =============================================================
 * keyboard.c - Implementacion del driver de teclado
 * =============================================================
 * El teclado, cada vez que apretas o soltas una tecla, dispara la
 * IRQ1 y deja un "scancode" en el puerto 0x60. El scancode NO es la
 * letra: es un numero que identifica la tecla fisica. Nosotros lo
 * traducimos a un caracter usando una tabla, y lo imprimimos.
 *
 * Nota: este es un teclado en layout US basico. Las teclas como n~,
 * acentos, etc. del teclado espanol no van a salir bien todavia
 * (eso requeriria un mapa de teclado mas completo). Es un primer
 * driver funcional, no uno completo.
 * ============================================================= */
#include "keyboard.h"
#include "irq.h"
#include "io.h"
#include "vga.h"
#include "shell.h"

#define KBD_DATA_PORT 0x60

/* Tabla de scancode -> caracter (sin shift), layout US. */
static const char scancode_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',   /* 0-9 */
    '9', '0', '-', '=', '\b',                          /* backspace */
    '\t',                                              /* tab */
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,                                                 /* ctrl izq */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,                                                 /* shift izq */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0,                                                 /* shift der */
    '*',
    0,                                                 /* alt */
    ' ',                                               /* espacio */
    0,                                                 /* caps lock */
    /* el resto (teclas F, etc) lo dejamos en 0 */
};

/* Tabla con shift apretado (mayusculas y simbolos). */
static const char scancode_map_shift[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',
    '(', ')', '_', '+', '\b',
    '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
    0,
    '*',
    0,
    ' ',
    0,
};

/* Estado de la tecla shift. */
static int shift_pressed = 0;

/* Manejador que se ejecuta en cada IRQ1 (tecla). */
static void keyboard_handler(struct regs* r) {
    (void) r; /* no usamos los registros aca */

    uint8_t scancode = inb(KBD_DATA_PORT);

    /* Si el bit alto esta encendido (>= 0x80), es una tecla SOLTADA. */
    if (scancode & 0x80) {
        uint8_t released = scancode & 0x7F;
        /* Shift izq (0x2A) o der (0x36) soltadas. */
        if (released == 0x2A || released == 0x36) {
            shift_pressed = 0;
        }
        return;
    }

    /* Tecla PRESIONADA. */
    /* Shift izq o der. */
    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = 1;
        return;
    }

    char c = shift_pressed ? scancode_map_shift[scancode]
                           : scancode_map[scancode];
    if (c) {
        shell_handle_char(c); /* pasarle la tecla a la shell */
    }
}

void keyboard_install(void) {
    irq_install_handler(1, keyboard_handler); /* IRQ1 = teclado */
}
