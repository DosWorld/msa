# MSA2 x86 assembler for real-mode DOS
     ____    ____   ______        _        _____   
    |_   \  /   _|.' ____ \      / \      / ___ `. 
      |   \/   |  | (___ \_|    / _ \    |_/___) | 
      | |\  /| |   _.____`.    / ___ \    .'____.' 
     _| |_\/_| |_ | \____) | _/ /   \ \_ / /_____  
    |_____||_____| \______.'|____| |____||_______| 

MSA2 is a old free x86 assembler. It supports all 8086 instructions and
most of the 80186/80286 instructions. It supports output formats:

* flat binary output **-fbin**
* .com file **-fcom**
* .exe file with tiny memory model **-ftexe**.
* .ovl overlay file with tiny memory model **-fovl** (this is the same as texe, but with symbol table).

Initialy written by a 14-year old Swedish programmer (in 2000-2001) in
Borland C++ 2.0. Now, MSA2 is ported to Watcom C.

**MSA2** is pretty fast and the only file you'll need to build a program is
MSA2.EXE (30 KB). It runs fine on my old 286. MSA2 does not require DPMI.

**MSA2W** - precompiled binary for Win32. You can compile DOS programs in
modern Windows without DosBox.

I have not tested MSA2 in FreeDOS yet, but there should not be any
difference between running MSA2 in a DOS box, real-mode MS-DOS or FreeDOS.

## Build MSA2

You need Watcom C compiler (or gcc - for linux). Just type (in original/src/)

    make

or

    wmake

## License

This software is published under MIT license. See LICENSE file.
