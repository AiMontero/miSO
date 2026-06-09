/* =============================================================
 * timer.h - Driver del PIT (Programmable Interval Timer)
 * =============================================================
 * El PIT dispara la IRQ0 a una frecuencia que nosotros elegimos.
 * Cada disparo es un "tick". Con eso medimos el tiempo y podemos
 * hacer pausas.
 * ============================================================= */
#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

/* Instala el timer a 'frequency' ticks por segundo (ej: 100). */
void timer_install(uint32_t frequency);

/* Devuelve cuantos ticks pasaron desde el arranque. */
uint32_t timer_get_ticks(void);

/* Pausa la ejecucion durante 'ms' milisegundos (aprox). */
void timer_sleep(uint32_t ms);

#endif
