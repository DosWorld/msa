# MSA2 x86 assembler for real-mode DOS
     ____    ____   ______        _        _____   
    |_   \  /   _|.' ____ \      / \      / ___ `. 
      |   \/   |  | (___ \_|    / _ \    |_/___) | 
      | |\  /| |   _.____`.    / ___ \    .'____.' 
     _| |_\/_| |_ | \____) | _/ /   \ \_ / /_____  
    |_____||_____| \______.'|____| |____||_______| 

MSA2 is a old and free x86 two-pass assembler for DOS targets. It supports all
Intel 8086 instructions and most of the 80186/80286 instructions.

Initialy written by a 14-year old Swedish programmer Robert Ostling (in 2000-2001)
in Borland C++ 2.0. Now, MSA2 is ported to Watcom C and GCC (and/or mingw).

**MSA2** Consume to small piece of memory - i think, will be enougth just 128K.
MSA2 does not rquire DPMI.

**MSA2** is pretty fast and the only file you'll need to build a program is
MSA2.EXE (30 KB). It runs fine on my old 286.

**MSA2W** - precompiled binary for win32. You can compile DOS programs in
modern windows without DosBox.

I had not test MSA2 in FreeDOS, Linux, windows yet, but there should not be any
difference where running MSA2.

## Output formats

**MSA2** produces next formats:

* flat binary output **-fbin**
* .com file **-fcom**
* .exe file with tiny memory model **-ftexe**.
* .ovl overlay file with tiny memory model **-fovl** (this is the same as texe, but with symbol table).

Examples how to use overlay - see /original/examples/OVL-TP7/ folder.

## Differences with NASM

* No macros / includes
* Arguments could not contain spaces (like "mov ax, b +1" - should be "mov ax,b+1")

## Build MSA2

MSA2 could be compiled for DOS, Linux and windows with Open Watcom C, GCC, GCC/MINGW.

### DOS

To build DOS version with Watcom C, just type (in original/src/)

    make

or

    wmake

### Linux

To build Linux version with GCC:

    gcc -O3 MSA.C TABLES.C ASSEMBLR.C MISC.C LEX.C -o MSA2
    strip MSA2

### windows

To build win32 version with mingw:

    i686-w64-mingw32-gcc -m32 -O3 MSA.C TABLES.C ASSEMBLR.C MISC.C LEX.C -o MSA2W.EXE
    strip MSA2W.EXE

## License

This is free software: you can redistribute it and/or modify it under the terms of the MIT license. See LICENSE file.
