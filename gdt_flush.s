# =============================================================
# gdt_flush.s - Carga la GDT y refresca los segmentos
# =============================================================
# Despues de cargar la GDT con lgdt, hay que recargar los registros
# de segmento (cs, ds, etc) para que apunten a las nuevas entradas.
# El selector 0x08 = entrada 1 (codigo). El 0x10 = entrada 2 (datos).
# (Cada entrada ocupa 8 bytes, por eso 0x08, 0x10, ...).
# =============================================================
.global gdt_flush
.type gdt_flush, @function
gdt_flush:
    mov 4(%esp), %eax    # tomar el argumento (puntero a la GDT)
    lgdt (%eax)          # cargar la GDT

    mov $0x10, %ax       # 0x10 = selector de datos del kernel
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    # Salto "lejano" para recargar cs con el selector de codigo (0x08).
    ljmp $0x08, $.flush
.flush:
    ret
