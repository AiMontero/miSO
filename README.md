# miSO 🖥️

Un sistema operativo educativo escrito desde cero en C y Assembly x86.

Bootea sobre hardware (real o emulado) sin ningún sistema operativo debajo, maneja interrupciones de hardware, lee el teclado, mide el tiempo, e incluye una mini-shell con un sistema de archivos en RAM.

Hecho con fines de aprendizaje, siguiendo de cerca la [OSDev Wiki](https://wiki.osdev.org/). El objetivo es que cualquiera pueda clonar el repo, seguir los pasos y tener su propio SO booteando.

---

## ✨ Características

- **Arranque Multiboot** vía GRUB.
- **GDT** (Global Descriptor Table) con modelo flat.
- **IDT** (Interrupt Descriptor Table) de 256 entradas.
- **PIC remapeado** + sistema de IRQs (puente Assembly ↔ C).
- **Driver de teclado** (layout US) con soporte de Shift y Backspace.
- **Timer / PIT** a 100 Hz: uptime y `sleep` reales.
- **Salida VGA** en modo texto con colores y scroll.
- **Mini-shell** que interpreta comandos.
- **Sistema de archivos en RAM** con operaciones CRUD.

### Comandos de la shell

| Comando | Descripción |
|---|---|
| `help` | Lista los comandos disponibles |
| `clear` | Limpia la pantalla |
| `uptime` | Segundos transcurridos desde el arranque |
| `echo <texto>` | Repite el texto |
| `about` | Información del sistema |
| `whoami` | Muestra el usuario |
| `ls` | Lista los archivos |
| `create <nombre>` | Crea un archivo vacío |
| `write <nombre> <texto>` | Escribe texto en un archivo (reemplaza) |
| `append <nombre> <texto>` | Agrega texto al final |
| `cat <nombre>` | Muestra el contenido de un archivo |
| `rm <nombre>` | Borra un archivo |

> ⚠️ El sistema de archivos vive en RAM: los archivos se pierden al apagar. La persistencia en disco requiere un driver de disco (ver Roadmap).

---

## 📋 Requisitos

- Una máquina Linux (recomendado: Ubuntu/Debian). Puede ser una VM.
- Un **cross-compiler `i686-elf`** (cómo armarlo, más abajo).
- QEMU para emular.
- GRUB y xorriso para armar la ISO booteable.

---

## 🚀 Instalación y uso

### 1. Instalar dependencias

```bash
sudo apt update
sudo apt install -y build-essential bison flex libgmp3-dev libmpc-dev \
  libmpfr-dev texinfo nasm qemu-system-x86 grub-pc-bin grub-common xorriso
```

### 2. Construir el cross-compiler `i686-elf`

Este es el paso más largo (se compila desde fuente). Se hace **una sola vez**.

```bash
export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
mkdir -p $HOME/src && cd $HOME/src

# --- binutils ---
wget https://ftp.gnu.org/gnu/binutils/binutils-2.42.tar.gz
tar xf binutils-2.42.tar.gz
mkdir build-binutils && cd build-binutils
../binutils-2.42/configure --target=$TARGET --prefix="$PREFIX" \
  --with-sysroot --disable-nls --disable-werror
make -j$(nproc)
make install
cd ..

# --- gcc ---
wget https://ftp.gnu.org/gnu/gcc/gcc-13.2.0/gcc-13.2.0.tar.gz
tar xf gcc-13.2.0.tar.gz
mkdir build-gcc && cd build-gcc
../gcc-13.2.0/configure --target=$TARGET --prefix="$PREFIX" \
  --disable-nls --enable-languages=c,c++ --without-headers
make -j$(nproc) all-gcc
make -j$(nproc) all-target-libgcc
make install-gcc
make install-target-libgcc
cd ..
```

Hacer el cross-compiler permanente (que esté siempre en el PATH):

```bash
echo 'export PATH="$HOME/opt/cross/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc
```

Verificar que quedó instalado:

```bash
i686-elf-gcc --version   # debería mostrar: i686-elf-gcc (GCC) 13.2.0
```

### 3. Compilar y arrancar miSO

```bash
git clone https://github.com/AiMontero/miSO.git
cd miSO
make run
```

Se abre QEMU con GRUB. Apretás **Enter** y arranca el sistema.

### Comandos del Makefile

| Comando | Acción |
|---|---|
| `make` | Compila el kernel (`myos.bin`) |
| `make iso` | Arma la ISO booteable (`myos.iso`) |
| `make run` | Compila, arma la ISO y arranca QEMU |
| `make clean` | Borra los archivos generados |

---

## 🐛 Errores comunes (y cómo resolverlos)

Estos son los tropiezos reales que aparecen al armar el proyecto. Si te trabás, mirá acá primero.

### `configure: No existe el archivo o el directorio`

**Causa:** el `wget` o el `tar xf` no se completaron, así que la carpeta de binutils/gcc no existe donde el `configure` la busca.

**Solución:** verificá que el archivo se descargó y descomprimió bien:

```bash
cd $HOME/src
ls -lh binutils-2.42.tar.gz   # debería pesar ~50 MB
ls -d binutils-2.42/          # la carpeta debe existir (sale en azul)
```

Si falta, volvé a correr el `wget` y el `tar xf`.

### `make: No se especificó ningún objetivo y no se encontró ningún makefile`

**Causa:** estás en una carpeta sin Makefile, o el `configure` anterior falló y no generó el Makefile.

**Solución:** confirmá que estás en la carpeta correcta y que los pasos previos terminaron bien.

### Las variables de entorno se perdieron

**Causa:** abriste una terminal nueva y los `export PREFIX/TARGET/PATH` no persisten entre sesiones.

**Solución:** revisá si están seteadas y, si no, volvé a exportarlas:

```bash
echo $PREFIX   # si sale vacío, hay que re-exportar
export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
```

### `i686-elf-gcc: command not found`

**Causa:** el PATH de la sesión no incluye el cross-compiler.

**Solución:** agregalo al `.bashrc` (ver paso 2) y corré `source ~/.bashrc`, o abrí una terminal nueva.

### El Makefile falla con `missing separator`

**Causa:** las líneas indentadas del Makefile usan **espacios** en vez de **TABs**. Make exige TABs.

**Solución:** convertí los espacios al inicio de línea en TABs:

```bash
sed -i 's/^  */\t/' Makefile
# verificar (las reglas deben empezar con ^I):
cat -A Makefile | grep '\^I'
```

### `make: Atención: ... una hora de modificación en el futuro` / `desviación en el reloj`

**Causa:** el reloj de la VM está desfasado respecto a la fecha de los archivos. Común en máquinas virtuales.

**Solución:** "tocá" todos los archivos para darles la hora actual:

```bash
touch *
```

(Opcional, para arreglar el reloj de raíz: `sudo hwclock --hctosys` o `sudo timedatectl set-ntp true`.)

### Booteo una versión vieja del SO (los cambios no aparecen)

**Causa:** `make` reutilizó un binario ya compilado y no recompiló con el código nuevo.

**Solución:** forzá una recompilación limpia:

```bash
make clean
touch *
make run
```

### Kernel panic al arrancar la VM de Ubuntu (no miSO, sino la VM host)

**Causa típica:** RAM insuficiente o configuración de CPU en VirtualBox. Ubuntu Desktop moderno (GNOME) puede pedir 6+ GB.

**Solución:** subí la RAM de la VM, activá PAE/NX y probá chipset ICH9. **Recomendación:** para este proyecto, usá **Ubuntu Server** (mucho más liviano, no necesitás escritorio).

### No funciona copiar/pegar entre el host y la VM

**Causa:** faltan las Guest Additions, o la sesión es Wayland (el portapapeles compartido anda mejor en X11).

**Solución:** instalá las Guest Additions (`Dispositivos → Insertar imagen de CD de Guest Additions`), o probá la sesión "Ubuntu on Xorg" en el login. Alternativa rápida: abrí el repo en un navegador dentro de la VM y copiá los archivos ahí.

### Las tildes y la ñ salen mal al escribir en la shell

**Causa:** el driver de teclado usa layout US y maneja ASCII básico.

**Solución:** es una limitación conocida; las letras a-z y los números funcionan perfecto. Un mapa de teclado latino completo está en el Roadmap.

### QEMU "captura" el mouse y no puedo salir

**Solución:** apretá `Ctrl+Alt+G` para liberar el mouse y el teclado.

---

## 📁 Estructura del proyecto

```
miSO/
├── boot.s          # Punto de entrada en Assembly (header Multiboot)
├── linker.ld       # Script del linker (layout en memoria)
├── grub.cfg        # Entrada de menú de GRUB
├── Makefile        # Automatización del build
│
├── kernel.c        # Kernel principal: inicializa todo
│
├── gdt.c/.h        # Global Descriptor Table
├── gdt_flush.s     # Carga de la GDT (Assembly)
├── idt.c/.h        # Interrupt Descriptor Table
├── idt_load.s      # Carga de la IDT (Assembly)
├── irq.c/.h        # PIC y ruteo de IRQs
├── irq_asm.s       # Puntos de entrada de las IRQs (Assembly)
│
├── timer.c/.h      # Driver del PIT (timer)
├── keyboard.c/.h   # Driver de teclado
├── vga.c/.h        # Salida en pantalla (modo texto VGA)
├── io.h            # Acceso a puertos de E/S
│
├── string.c/.h     # Utilidades de strings
├── shell.c/.h      # Mini-shell (intérprete de comandos)
└── fs.c/.h         # Sistema de archivos en RAM
```

---

## 🗺️ Roadmap

Próximas funcionalidades posibles (contribuciones bienvenidas):

- [ ] Heap del kernel (`malloc` / `free`)
- [ ] Excepciones de CPU (ISRs 0-31): división por cero, page faults, etc.
- [ ] Paginación / memoria virtual
- [ ] Driver de disco + filesystem persistente
- [ ] Editor de texto de pantalla completa
- [ ] Multitarea (scheduler basado en el timer)
- [ ] Mapa de teclado latino (ñ, acentos)

---

## 🤝 Contribuir

Las contribuciones son bienvenidas. Si encontrás un bug o querés agregar una funcionalidad:

1. Hacé un fork del repo.
2. Creá una rama (`git checkout -b mi-feature`).
3. Commiteá tus cambios (`git commit -m 'Agrego mi-feature'`).
4. Pusheá la rama (`git push origin mi-feature`).
5. Abrí un Pull Request.

---

## 📚 Recursos

- [OSDev Wiki](https://wiki.osdev.org/) — la referencia principal, especialmente las páginas "Bare Bones" y "Meaty Skeleton".
- [Operating Systems: Three Easy Pieces](https://pages.cs.wisc.edu/~remzi/OSTEP/) — teoría de sistemas operativos (gratis).
- [xv6](https://pdos.csail.mit.edu/6.828/) — un SO didáctico de MIT con código legible.

---

## 📄 Licencia

Este proyecto está bajo la licencia MIT. Ver el archivo [LICENSE](LICENSE) para más detalles.

---

Hecho con unos vasos de Coca Light para alimentar el tanque, y trasnochada montevideana mezclada con mucha curiosidad y cariño, por **Alfredo Montero**.
