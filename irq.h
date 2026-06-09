/* =============================================================
 * irq.h - Manejo de IRQs (interrupciones de hardware)
 * =============================================================
 * Una IRQ es una interrupcion que dispara un dispositivo fisico
 * (teclado=IRQ1, timer=IRQ0, etc). El chip PIC las recibe y se las
 * pasa a la CPU. Aca configuramos todo eso.
 * ============================================================= */
#ifndef IRQ_H
#define IRQ_H

#include <stdint.h>

/* Estructura con el estado de los registros al momento de la
 * interrupcion. Nos la arma el codigo Assembly. */
struct regs {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
};

/* Inicializa el PIC y prepara las IRQs. */
void irq_install(void);

/* Registra una funcion C como manejador de una IRQ concreta. */
void irq_install_handler(int irq, void (*handler)(struct regs* r));

#endif
