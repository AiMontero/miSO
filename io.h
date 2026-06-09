/* =============================================================
 * io.h - Acceso a puertos de E/S (entrada/salida) del hardware
 * =============================================================
 * En x86, los dispositivos (teclado, PIC, timer) se controlan
 * leyendo y escribiendo en "puertos". No son direcciones de memoria
 * normales: se accede con instrucciones especiales de Assembly
 * (in / out). Aca las envolvemos en funciones C comodas.
 * ============================================================= */
#ifndef IO_H
#define IO_H

#include <stdint.h>

/* Escribe un byte 'val' en el puerto 'port'. */
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

/* Lee un byte desde el puerto 'port'. */
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* Pequena espera de E/S. Algunos chips viejos (como el PIC) necesitan
 * un instante entre comandos. Escribir en el puerto 0x80 (sin uso real)
 * gasta el tiempo justo. */
static inline void io_wait(void) {
    outb(0x80, 0);
}

#endif
