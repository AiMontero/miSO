# =============================================================
# irq_asm.s - Puntos de entrada en Assembly para las IRQs
# =============================================================
# Cuando ocurre una IRQ, la CPU salta aca. Cada IRQ guarda su numero
# y salta al codigo comun, que guarda el estado de los registros y
# llama a irq_handler (en C). Despues restaura todo y vuelve.
# =============================================================

# Macro: define una IRQ que no empuja codigo de error.
# Empuja un 0 de relleno + el numero de interrupcion (32 + n).
.macro IRQ num, intno
.global irq\num
irq\num:
    cli
    push $0
    push $\intno
    jmp irq_common_stub
.endm

# Definir las 16 IRQs (interrupciones 32 a 47).
IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

# Esta funcion C la definimos en irq.c.
.extern irq_handler

# Codigo comun: guarda todo el estado, llama a C, restaura y vuelve.
irq_common_stub:
    pusha                 # guardar eax,ecx,edx,ebx,esp,ebp,esi,edi

    mov %ds, %ax          # guardar el segmento de datos actual
    push %eax

    mov $0x10, %ax        # cargar el segmento de datos del kernel
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    push %esp             # pasar puntero a struct regs como argumento
    call irq_handler
    add $4, %esp          # limpiar el argumento

    pop %eax              # restaurar el segmento de datos original
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    popa                  # restaurar registros
    add $8, %esp          # limpiar el codigo de error y el numero
    sti
    iret                  # retorno de interrupcion
