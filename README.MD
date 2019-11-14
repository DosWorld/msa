     ____    ____   ______        _        _____   
    |_   \  /   _|.' ____ \      / \      / ___ `. 
      |   \/   |  | (___ \_|    / _ \    |_/___) | 
      | |\  /| |   _.____`.    / ___ \    .'____.' 
     _| |_\/_| |_ | \____) | _/ /   \ \_ / /_____  
    |_____||_____| \______.'|____| |____||_______| 

# MSA2 x86 assembler for real-mode DOS

MSA2 is a old free x86 assembler. It supports all 8086 instructions and most of the 80186/80286 instructions. It supports output formats:

* flat binary output **-fbin**
* .com file **-fcom**
* .exe file with tiny memory model **-ftexe**.
* .ovl overlay file with tiny memory model **-fovl** (this is the same as texe, but with symbol table).

Initialy written by a 14-year old Swedish programmer (in 2001).

MSA2 is pretty fast, much faster than NASM, and the only file you'll need to build a program is MSA2.EXE (35 KB). It runs fine on my old 286. Also, MSA2 does not require DPMI.

I wrote MSA2 in C, using Borland C++ 2.0, but I don't think it should be very hard to port to another compiler. The source code is not commented, not structured, so, it may be a little hard to read...
Now, MSA2 is ported to Watcom C.

I have not tested MSA2 in FreeDOS yet, but there should not be any difference between running MSA2 in a DOS box, real-mode MS-DOS or FreeDOS.

## Build MSA2

You need Watcom C compiler. Just type (in original/src/)

    make

or

    wmake

## License

This software is published under MIT license. See LICENSE file.
