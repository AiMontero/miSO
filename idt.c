/* =============================================================
 * idt.c - Implementacion de la IDT
 * =============================================================
 * Construimos la tabla de 256 entradas y la cargamos con 'lidt'.
 * Por ahora la dejamos vacia; los manejadores concretos (teclado,
 * etc.) se registran despues con idt_set_gate.
 * ============================================================= */
#include "idt.h"

/* Una entrada de la IDT (8 bytes). */
struct idt_entry {
    uint16_t base_low;   /* parte baja de la direccion del manejador */
    uint16_t sel;        /* selector de segmento de codigo (0x08) */
    uint8_t  always0;    /* siempre 0 */
    uint8_t  flags;      /* tipo y atributos */
    uint16_t base_high;  /* parte alta de la direccion del manejador */
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

static struct idt_entry idt[256];
static struct idt_ptr   ip;

/* Funcion en Assembly (idt_load.s) que ejecuta lidt. */
extern void idt_load(uint32_t);

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low  = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].sel       = sel;
    idt[num].always0   = 0;
    idt[num].flags     = flags;
}

void idt_install(void) {
    ip.limit = (sizeof(struct idt_entry) * 256) - 1;
    ip.base  = (uint32_t) &idt;

    /* Limpiar toda la tabla a cero. */
    for (int i = 0; i < 256; i++) {
        idt[i].base_low  = 0;
        idt[i].base_high = 0;
        idt[i].sel       = 0;
        idt[i].always0   = 0;
        idt[i].flags     = 0;
    }

    idt_load((uint32_t) &ip);
}
