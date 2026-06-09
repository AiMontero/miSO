/* =============================================================
 * timer.c - Implementacion del PIT
 * =============================================================
 * El PIT corre a una frecuencia base fija de 1193182 Hz. Para que
 * dispare a la frecuencia que queremos (ej: 100 Hz), le cargamos un
 * "divisor": divisor = 1193182 / frecuencia_deseada. El chip cuenta
 * hasta ese divisor y recien ahi dispara la IRQ0.
 * ============================================================= */
#include "timer.h"
#include "irq.h"
#include "io.h"
#include "vga.h"

/* Frecuencia base del PIT en Hz (es un valor de hardware fijo). */
#define PIT_BASE_FREQUENCY 1193182

/* Puertos del PIT. */
#define PIT_CHANNEL0 0x40
#define PIT_COMMAND  0x43

/* Contador global de ticks desde el arranque. */
static volatile uint32_t tick_count = 0;

/* Frecuencia configurada (ticks por segundo), para calcular tiempos. */
static uint32_t timer_frequency = 0;

/* Manejador que se ejecuta en cada IRQ0 (cada tick). */
static void timer_handler(struct regs* r) {
    (void) r;
    tick_count++;
}

uint32_t timer_get_ticks(void) {
    return tick_count;
}

void timer_install(uint32_t frequency) {
    timer_frequency = frequency;

    /* Calcular el divisor para la frecuencia deseada. */
    uint32_t divisor = PIT_BASE_FREQUENCY / frequency;

    /* Comando 0x36: canal 0, acceso lo/hi byte, modo onda cuadrada. */
    outb(PIT_COMMAND, 0x36);

    /* Enviar el divisor partido en byte bajo y byte alto. */
    outb(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xFF));

    /* Registrar nuestro manejador en la IRQ0. */
    irq_install_handler(0, timer_handler);
}

void timer_sleep(uint32_t ms) {
    if (timer_frequency == 0) return; /* timer no instalado todavia */

    /* Cuantos ticks equivalen a 'ms' milisegundos. */
    uint32_t ticks_to_wait = (timer_frequency * ms) / 1000;
    uint32_t start = tick_count;

    /* Esperar activamente hasta que pasen los ticks. El 'hlt' deja
     * dormir la CPU entre tick y tick para no gastar el 100% de CPU. */
    while ((tick_count - start) < ticks_to_wait) {
        asm volatile ("hlt");
    }
}
