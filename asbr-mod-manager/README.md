# ASBR Mod Manager (Switch)

Activa/desactiva mods CPK de JoJo's Bizarre Adventure: All-Star Battle R.

## Uso
1. Crea `/switch/asbr-mods/<nombre_mod>/` en la SD.
2. Mete dentro el/los `.cpk` y un `mod.json` (ver `example-mod/`).
   - `target`: `patch` o `launch` (segun lo que pida el mod).
   - Si omites `files`, se usan todos los `.cpk` de la carpeta.
3. Abre la app (homebrew) y activa el mod con el switch.

Activar = copia el CPK a
`/atmosphere/contents/010017A0128C4000/romfs/data/<target>/`.
Si otro mod activo usa el mismo nombre de CPK, o el archivo ya existe
en destino, se avisa y no se copia nada.

## Compilar (devkitPro)
```
git init && git submodule add https://github.com/xfangfang/borealis library/borealis
cmake -B build -DCMAKE_TOOLCHAIN_FILE=$DEVKITPRO/cmake/Switch.cmake
cmake --build build
```
Salida: `build/asbr_mod_manager.nro` -> copiar a `/switch/` en la SD.

## Notas
- Codigo sin compilar ni probar en hardware: la API de Borealis cambia entre
  versiones, asi que puede que haya que ajustar `main_activity.cpp` y el CMake
  (mira el demo de Borealis como referencia).
- Copiar CPKs grandes tarda; no hay barra de progreso todavia.
- Pendiente: herramienta de PC para fusionar CPKs (PyCriCodecs).
