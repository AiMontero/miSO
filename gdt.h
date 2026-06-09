/* =============================================================
 * gdt.h - Global Descriptor Table (GDT)
 * =============================================================
 * La GDT le dice a la CPU como estan organizados los segmentos de
 * memoria (codigo, datos) y con que permisos. En SOs modernos se usa
 * un modelo "flat": un solo segmento que cubre TODA la memoria, asi
 * trabajamos con direcciones lineales simples. Pero la CPU igual
 * exige que la GDT exista y este cargada.
 * ============================================================= */
#ifndef GDT_H
#define GDT_H

void gdt_install(void);

#endif
