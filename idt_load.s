# =============================================================
# idt_load.s - Carga la IDT con la instruccion lidt
# =============================================================
.global idt_load
.type idt_load, @function
idt_load:
    mov 4(%esp), %eax    # tomar el argumento (puntero a la IDT)
    lidt (%eax)          # cargar la IDT
    ret
