/* =============================================================
 * fs.c - Implementacion del sistema de archivos en RAM
 * =============================================================
 * Cada archivo es una entrada en el array 'files'. El campo 'used'
 * indica si la entrada esta ocupada. Todo en memoria estatica, sin
 * heap: simple y suficiente para empezar.
 * ============================================================= */
#include "fs.h"
#include "string.h"

/* Una entrada del sistema de archivos. */
struct file {
    int  used;                      /* 1 si esta ocupada, 0 si libre */
    char name[FS_MAX_NAME];         /* nombre del archivo */
    char content[FS_MAX_CONTENT];   /* contenido */
    int  size;                      /* tamano actual del contenido */
};

static struct file files[FS_MAX_FILES];

void fs_init(void) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        files[i].used = 0;
        files[i].size = 0;
        files[i].name[0] = '\0';
        files[i].content[0] = '\0';
    }
}

/* Busca un archivo por nombre. Devuelve su indice, o -1 si no existe. */
static int find_file(const char* name) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (files[i].used && strcmp(files[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

/* Busca una entrada libre. Devuelve su indice, o -1 si esta lleno. */
static int find_free(void) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (!files[i].used) {
            return i;
        }
    }
    return -1;
}

enum fs_result fs_create(const char* name) {
    if (strlen(name) >= FS_MAX_NAME) {
        return FS_ERR_NAME_TOO_LONG;
    }
    if (find_file(name) != -1) {
        return FS_ERR_EXISTS;
    }
    int idx = find_free();
    if (idx == -1) {
        return FS_ERR_FULL;
    }
    files[idx].used = 1;
    files[idx].size = 0;
    strcpy(files[idx].name, name);
    files[idx].content[0] = '\0';
    return FS_OK;
}

enum fs_result fs_write(const char* name, const char* content) {
    int idx = find_file(name);
    if (idx == -1) {
        return FS_ERR_NOT_FOUND;
    }
    int len = strlen(content);
    if (len >= FS_MAX_CONTENT) {
        return FS_ERR_TOO_BIG;
    }
    strcpy(files[idx].content, content);
    files[idx].size = len;
    return FS_OK;
}

enum fs_result fs_append(const char* name, const char* content) {
    int idx = find_file(name);
    if (idx == -1) {
        return FS_ERR_NOT_FOUND;
    }
    int current = files[idx].size;
    int extra = strlen(content);
    if (current + extra >= FS_MAX_CONTENT) {
        return FS_ERR_TOO_BIG;
    }
    /* Copiar el texto nuevo a partir del final del contenido actual. */
    strcpy(files[idx].content + current, content);
    files[idx].size = current + extra;
    return FS_OK;
}

enum fs_result fs_delete(const char* name) {
    int idx = find_file(name);
    if (idx == -1) {
        return FS_ERR_NOT_FOUND;
    }
    files[idx].used = 0;
    files[idx].size = 0;
    files[idx].name[0] = '\0';
    files[idx].content[0] = '\0';
    return FS_OK;
}

const char* fs_read(const char* name) {
    int idx = find_file(name);
    if (idx == -1) {
        return 0; /* NULL */
    }
    return files[idx].content;
}

void fs_list(void (*callback)(const char* name, int size)) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (files[i].used) {
            callback(files[i].name, files[i].size);
        }
    }
}
