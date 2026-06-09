/* =============================================================
 * shell.h - Mini-shell (interprete de comandos)
 * =============================================================
 * Acumula lo que escribis y, al apretar Enter, interpreta la linea
 * como un comando.
 * ============================================================= */
#ifndef SHELL_H
#define SHELL_H

/* Inicializa la shell (muestra el primer prompt). */
void shell_init(void);

/* Recibe un caracter tecleado y lo procesa (lo agrega al buffer,
 * o ejecuta el comando si es Enter, o borra si es backspace). */
void shell_handle_char(char c);

#endif
