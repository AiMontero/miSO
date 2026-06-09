/* =============================================================
 * string.h - Utilidades basicas de strings
 * =============================================================
 * Como no tenemos la libreria estandar de C, nos armamos las pocas
 * funciones de strings que la shell necesita.
 * ============================================================= */
#ifndef STRING_H
#define STRING_H

#include <stddef.h>

/* Compara dos strings. Devuelve 0 si son iguales. */
int strcmp(const char* a, const char* b);

/* Compara los primeros n caracteres. Devuelve 0 si coinciden. */
int strncmp(const char* a, const char* b, size_t n);

/* Longitud de un string. */
size_t strlen(const char* s);

/* Copia src en dest (incluido el \0). Devuelve dest. */
char* strcpy(char* dest, const char* src);

/* Copia hasta n caracteres de src en dest. */
char* strncpy(char* dest, const char* src, size_t n);

#endif
