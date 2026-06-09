/* =============================================================
 * idt.h - Interrupt Descriptor Table (IDT)
 * =============================================================
 * La IDT es la tabla que le dice a la CPU: "cuando ocurra la
 * interrupcion numero N, ejecuta la funcion que esta en esta
 * direccion". Hay 256 posibles interrupciones (0 a 255).
 * ============================================================= */
#ifndef IDT_H
#define IDT_H

#include <stdint.h>

void idt_install(void);
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

#endif
