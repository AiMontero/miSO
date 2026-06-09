/* =============================================================
 * gdt.c - Implementacion de la GDT
 * =============================================================
 * Definimos 5 entradas:
 *   0: nula (obligatoria por la arquitectura)
 *   1: segmento de codigo del kernel
 *   2: segmento de datos del kernel
 *   3: segmento de codigo de usuario (lo dejamos listo para el futuro)
 *   4: segmento de datos de usuario (idem)
 * Todas en modelo "flat": base 0, limite 4GB.
 * ============================================================= */
#include <stdint.h>

/* Una entrada de la GDT (8 bytes). Los campos estan partidos en
 * pedazos raros por razones historicas del x86. */
struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

/* El puntero que la instruccion 'lgdt' necesita: tamano + direccion. */
struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

static struct gdt_entry gdt[5];
static struct gdt_ptr   gp;

/* Funcion en Assembly (en gdt_flush.s) que carga la GDT y refresca
 * los registros de segmento. */
extern void gdt_flush(uint32_t);

/* Rellena una entrada de la GDT con los valores dados. */
static void gdt_set_gate(int num, uint32_t base, uint32_t limit,
                         uint8_t access, uint8_t gran) {
    gdt[num].base_low    = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high   = (base >> 24) & 0xFF;

    gdt[num].limit_low   = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0x0F;

    gdt[num].granularity |= gran & 0xF0;
    gdt[num].access      = access;
}

void gdt_install(void) {
    gp.limit = (sizeof(struct gdt_entry) * 5) - 1;
    gp.base  = (uint32_t) &gdt;

    /* 0: descriptor nulo. */
    gdt_set_gate(0, 0, 0, 0, 0);
    /* 1: codigo kernel. base=0, limite=4GB, ejecutable, anillo 0. */
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    /* 2: datos kernel. */
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    /* 3: codigo usuario (anillo 3). */
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    /* 4: datos usuario. */
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    /* Cargar la nueva GDT. */
    gdt_flush((uint32_t) &gp);
}
