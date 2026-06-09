/* =============================================================
 * fs.h - Sistema de archivos en RAM (ramdisk)
 * =============================================================
 * Un sistema de archivos MUY simple que vive enteramente en memoria.
 * Los archivos se guardan en un array fijo. Se pueden crear, escribir,
 * leer, listar y borrar. Limitacion: al apagar el SO, se pierden
 * (no persisten en disco; eso requeriria un driver de disco).
 * ============================================================= */
#ifndef FS_H
#define FS_H

#define FS_MAX_FILES    16    /* cantidad maxima de archivos */
#define FS_MAX_NAME     32    /* largo maximo del nombre */
#define FS_MAX_CONTENT  256   /* largo maximo del contenido */

/* Codigos de resultado de las operaciones. */
enum fs_result {
    FS_OK = 0,
    FS_ERR_EXISTS,       /* el archivo ya existe */
    FS_ERR_NOT_FOUND,    /* no se encontro el archivo */
    FS_ERR_FULL,         /* no hay espacio para mas archivos */
    FS_ERR_TOO_BIG,      /* el contenido no entra */
    FS_ERR_NAME_TOO_LONG /* el nombre es muy largo */
};

void fs_init(void);

enum fs_result fs_create(const char* name);
enum fs_result fs_write(const char* name, const char* content);
enum fs_result fs_append(const char* name, const char* content);
enum fs_result fs_delete(const char* name);

/* Devuelve el contenido de un archivo, o NULL si no existe. */
const char* fs_read(const char* name);

/* Lista los archivos llamando a una funcion 'callback' por cada uno.
 * (le pasa nombre y tamano). */
void fs_list(void (*callback)(const char* name, int size));

#endif
