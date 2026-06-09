/* =============================================================
 * shell.c - Implementacion de la mini-shell (v0.5)
 * =============================================================
 * Ahora con whoami y un sistema de archivos en RAM:
 *   create, write, append, cat, ls, rm
 * ============================================================= */
#include "shell.h"
#include "vga.h"
#include "string.h"
#include "timer.h"
#include "fs.h"

#define BUFFER_SIZE 256

static char buffer[BUFFER_SIZE];
static size_t buffer_len = 0;

static void print_prompt(void) {
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    terminal_writestring("miSO");
    terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    terminal_writestring("> ");
}

/* Helpers de color para mensajes. */
static void msg(uint8_t fg, const char* s) {
    terminal_setcolor(vga_entry_color(fg, VGA_COLOR_BLACK));
    terminal_writestring(s);
}

/* ---------- Comandos basicos ---------- */

static void cmd_help(void) {
    msg(VGA_COLOR_LIGHT_GREY, "Comandos disponibles:\n");
    terminal_writestring("  help              - muestra esta ayuda\n");
    terminal_writestring("  clear             - limpia la pantalla\n");
    terminal_writestring("  uptime            - segundos desde el arranque\n");
    terminal_writestring("  echo <texto>      - repite el texto\n");
    terminal_writestring("  about             - informacion del sistema\n");
    terminal_writestring("  whoami            - muestra el usuario\n");
    terminal_writestring("  --- archivos (en RAM) ---\n");
    terminal_writestring("  ls                - lista los archivos\n");
    terminal_writestring("  create <nombre>   - crea un archivo vacio\n");
    terminal_writestring("  write <nom> <txt> - escribe texto (reemplaza)\n");
    terminal_writestring("  append <nom> <txt>- agrega texto al final\n");
    terminal_writestring("  cat <nombre>      - muestra el contenido\n");
    terminal_writestring("  rm <nombre>       - borra un archivo\n");
}

static void cmd_clear(void) {
    terminal_initialize();
}

static void cmd_uptime(void) {
    uint32_t ticks = timer_get_ticks();
    uint32_t segundos = ticks / 100;
    msg(VGA_COLOR_LIGHT_GREY, "Uptime: ");
    terminal_writedec(segundos);
    terminal_writestring(" segundos (");
    terminal_writedec(ticks);
    terminal_writestring(" ticks)\n");
}

static void cmd_echo(const char* args) {
    msg(VGA_COLOR_LIGHT_GREY, args);
    terminal_writestring("\n");
}

static void cmd_about(void) {
    msg(VGA_COLOR_LIGHT_CYAN, "miSO v0.5 - un SO desde cero hecho por Alfredo Montero.\n");
    msg(VGA_COLOR_LIGHT_GREY, "Kernel propio: GDT, IDT, IRQs, timer (PIT),\n");
    terminal_writestring("teclado, mini-shell y sistema de archivos en RAM.\n");
}

static void cmd_whoami(void) {
    msg(VGA_COLOR_LIGHT_GREEN, "alfredo\n");
}

/* ---------- Comandos de archivos ---------- */

/* Callback para ls: imprime "  nombre (N bytes)". */
static void ls_callback(const char* name, int size) {
    terminal_writestring("  ");
    terminal_writestring(name);
    terminal_writestring(" (");
    terminal_writedec((uint32_t) size);
    terminal_writestring(" bytes)\n");
}

static int any_files = 0;
static void count_callback(const char* name, int size) {
    (void) name; (void) size;
    any_files = 1;
}

static void cmd_ls(void) {
    any_files = 0;
    fs_list(count_callback);
    if (!any_files) {
        msg(VGA_COLOR_DARK_GREY, "(no hay archivos)\n");
        return;
    }
    msg(VGA_COLOR_LIGHT_GREY, "Archivos:\n");
    fs_list(ls_callback);
}

/* Imprime un mensaje segun el codigo de error del fs. */
static void print_fs_error(enum fs_result r) {
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
    switch (r) {
        case FS_ERR_EXISTS:        terminal_writestring("Error: el archivo ya existe.\n"); break;
        case FS_ERR_NOT_FOUND:     terminal_writestring("Error: archivo no encontrado.\n"); break;
        case FS_ERR_FULL:          terminal_writestring("Error: no hay espacio para mas archivos.\n"); break;
        case FS_ERR_TOO_BIG:       terminal_writestring("Error: el contenido es muy grande.\n"); break;
        case FS_ERR_NAME_TOO_LONG: terminal_writestring("Error: el nombre es muy largo.\n"); break;
        default: break;
    }
}

static void cmd_create(const char* name) {
    if (name[0] == '\0') { msg(VGA_COLOR_LIGHT_RED, "Uso: create <nombre>\n"); return; }
    enum fs_result r = fs_create(name);
    if (r == FS_OK) {
        msg(VGA_COLOR_LIGHT_GREEN, "Archivo creado: ");
        terminal_writestring(name);
        terminal_writestring("\n");
    } else {
        print_fs_error(r);
    }
}

static void cmd_rm(const char* name) {
    if (name[0] == '\0') { msg(VGA_COLOR_LIGHT_RED, "Uso: rm <nombre>\n"); return; }
    enum fs_result r = fs_delete(name);
    if (r == FS_OK) {
        msg(VGA_COLOR_LIGHT_GREY, "Archivo borrado: ");
        terminal_writestring(name);
        terminal_writestring("\n");
    } else {
        print_fs_error(r);
    }
}

static void cmd_cat(const char* name) {
    if (name[0] == '\0') { msg(VGA_COLOR_LIGHT_RED, "Uso: cat <nombre>\n"); return; }
    const char* content = fs_read(name);
    if (content == 0) {
        print_fs_error(FS_ERR_NOT_FOUND);
        return;
    }
    msg(VGA_COLOR_WHITE, content);
    terminal_writestring("\n");
}

/* write y append reciben "nombre texto...". Hay que separar el primer
 * token (nombre) del resto (texto). */
static void cmd_write_or_append(const char* args, int append) {
    /* Buscar el espacio que separa nombre y texto. */
    int space = -1;
    for (int i = 0; args[i] != '\0'; i++) {
        if (args[i] == ' ') { space = i; break; }
    }

    if (space == -1) {
        msg(VGA_COLOR_LIGHT_RED, append ? "Uso: append <nombre> <texto>\n"
                                        : "Uso: write <nombre> <texto>\n");
        return;
    }

    /* Separar nombre (hasta el espacio) y texto (despues del espacio). */
    static char name[64];
    int i;
    for (i = 0; i < space && i < 63; i++) {
        name[i] = args[i];
    }
    name[i] = '\0';
    const char* text = args + space + 1;

    enum fs_result r = append ? fs_append(name, text) : fs_write(name, text);
    if (r == FS_OK) {
        msg(VGA_COLOR_LIGHT_GREEN, append ? "Texto agregado a " : "Texto escrito en ");
        terminal_writestring(name);
        terminal_writestring("\n");
    } else {
        print_fs_error(r);
    }
}

/* ---------- El interprete ---------- */

static void execute_command(void) {
    if (buffer_len == 0) return;
    buffer[buffer_len] = '\0';

    if (strcmp(buffer, "help") == 0) {
        cmd_help();
    } else if (strcmp(buffer, "clear") == 0) {
        cmd_clear();
    } else if (strcmp(buffer, "uptime") == 0) {
        cmd_uptime();
    } else if (strcmp(buffer, "about") == 0) {
        cmd_about();
    } else if (strcmp(buffer, "whoami") == 0) {
        cmd_whoami();
    } else if (strcmp(buffer, "ls") == 0) {
        cmd_ls();
    } else if (strncmp(buffer, "echo ", 5) == 0) {
        cmd_echo(buffer + 5);
    } else if (strcmp(buffer, "echo") == 0) {
        cmd_echo("");
    } else if (strncmp(buffer, "create ", 7) == 0) {
        cmd_create(buffer + 7);
    } else if (strncmp(buffer, "rm ", 3) == 0) {
        cmd_rm(buffer + 3);
    } else if (strncmp(buffer, "cat ", 4) == 0) {
        cmd_cat(buffer + 4);
    } else if (strncmp(buffer, "write ", 6) == 0) {
        cmd_write_or_append(buffer + 6, 0);
    } else if (strncmp(buffer, "append ", 7) == 0) {
        cmd_write_or_append(buffer + 7, 1);
    } else {
        msg(VGA_COLOR_LIGHT_RED, "Comando no reconocido: ");
        terminal_writestring(buffer);
        terminal_writestring("\n");
        msg(VGA_COLOR_LIGHT_GREY, "Escribi 'help' para ver los comandos.\n");
    }
}

void shell_init(void) {
    buffer_len = 0;
    print_prompt();
}

void shell_handle_char(char c) {
    if (c == '\n') {
        terminal_putchar('\n');
        execute_command();
        buffer_len = 0;
        print_prompt();
        return;
    }
    if (c == '\b') {
        if (buffer_len > 0) {
            buffer_len--;
            terminal_putchar('\b');
        }
        return;
    }
    if (buffer_len < BUFFER_SIZE - 1) {
        buffer[buffer_len++] = c;
        terminal_putchar(c);
    }
}
