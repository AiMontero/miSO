/* =============================================================
 * irq.c - Configuracion del PIC y ruteo de IRQs
 * =============================================================
 * El PIC (Programmable Interrupt Controller) es el chip que junta
 * las interrupciones de los dispositivos y se las manda a la CPU.
 * Problema: por defecto, sus IRQs chocan con las interrupciones que
 * el procesador reserva para errores (excepciones 0-31). Por eso hay
 * que "remapearlo": mover las IRQs a los numeros 32-47.
 * ============================================================= */
#include "irq.h"
#include "idt.h"
#include "io.h"

/* Puertos de los dos PICs (maestro y esclavo). */
#define PIC1_CMD  0x20
#define PIC1_DATA 0x21
#define PIC2_CMD  0xA0
#define PIC2_DATA 0xA1

/* Tabla de punteros a los manejadores C de cada IRQ (16 en total). */
static void (*irq_routines[16])(struct regs* r) = { 0 };

/* Estas funciones estan en irq_asm.s: son los puntos de entrada en
 * Assembly para cada IRQ. */
extern void irq0(); extern void irq1(); extern void irq2();
extern void irq3(); extern void irq4(); extern void irq5();
extern void irq6(); extern void irq7(); extern void irq8();
extern void irq9(); extern void irq10(); extern void irq11();
extern void irq12(); extern void irq13(); extern void irq14();
extern void irq15();

/* Registrar un manejador C para una IRQ. */
void irq_install_handler(int irq, void (*handler)(struct regs* r)) {
    irq_routines[irq] = handler;
}

/* Remapea el PIC para que las IRQs 0-15 lleguen como interrupciones
 * 32-47, evitando el choque con las excepciones de la CPU. */
static void pic_remap(void) {
    outb(PIC1_CMD, 0x11); io_wait();   /* iniciar secuencia */
    outb(PIC2_CMD, 0x11); io_wait();
    outb(PIC1_DATA, 0x20); io_wait();  /* PIC1 arranca en 32 */
    outb(PIC2_DATA, 0x28); io_wait();  /* PIC2 arranca en 40 */
    outb(PIC1_DATA, 0x04); io_wait();  /* decirle al PIC1 que hay esclavo en IRQ2 */
    outb(PIC2_DATA, 0x02); io_wait();
    outb(PIC1_DATA, 0x01); io_wait();  /* modo 8086 */
    outb(PIC2_DATA, 0x01); io_wait();
    outb(PIC1_DATA, 0x0); io_wait();   /* limpiar mascaras */
    outb(PIC2_DATA, 0x0); io_wait();
}

void irq_install(void) {
    pic_remap();

    /* Registrar las 16 entradas de IRQ en la IDT (numeros 32-47).
     * 0x08 = selector de codigo; 0x8E = interrupcion de anillo 0. */
    idt_set_gate(32, (uint32_t) irq0, 0x08, 0x8E);
    idt_set_gate(33, (uint32_t) irq1, 0x08, 0x8E);
    idt_set_gate(34, (uint32_t) irq2, 0x08, 0x8E);
    idt_set_gate(35, (uint32_t) irq3, 0x08, 0x8E);
    idt_set_gate(36, (uint32_t) irq4, 0x08, 0x8E);
    idt_set_gate(37, (uint32_t) irq5, 0x08, 0x8E);
    idt_set_gate(38, (uint32_t) irq6, 0x08, 0x8E);
    idt_set_gate(39, (uint32_t) irq7, 0x08, 0x8E);
    idt_set_gate(40, (uint32_t) irq8, 0x08, 0x8E);
    idt_set_gate(41, (uint32_t) irq9, 0x08, 0x8E);
    idt_set_gate(42, (uint32_t) irq10, 0x08, 0x8E);
    idt_set_gate(43, (uint32_t) irq11, 0x08, 0x8E);
    idt_set_gate(44, (uint32_t) irq12, 0x08, 0x8E);
    idt_set_gate(45, (uint32_t) irq13, 0x08, 0x8E);
    idt_set_gate(46, (uint32_t) irq14, 0x08, 0x8E);
    idt_set_gate(47, (uint32_t) irq15, 0x08, 0x8E);
}

/* Esta funcion la llama el Assembly cada vez que ocurre una IRQ.
 * Busca si hay un manejador C registrado y lo ejecuta. */
void irq_handler(struct regs* r) {
    void (*handler)(struct regs* r);

    /* int_no viene como 32-47; restamos 32 para indexar 0-15. */
    handler = irq_routines[r->int_no - 32];
    if (handler) {
        handler(r);
    }

    /* Avisar al PIC que terminamos (EOI = End Of Interrupt).
     * Si fue una IRQ del PIC esclavo (>= 40), avisarle tambien. */
    if (r->int_no >= 40) {
        outb(PIC2_CMD, 0x20);
    }
    outb(PIC1_CMD, 0x20);
}
