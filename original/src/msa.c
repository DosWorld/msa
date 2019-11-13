/*

MIT License

Copyright (c) 2000, 2001, 2019 Robert Ostling
Copyright (c) 2019 DosWorld

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

        Part of the MSA assembler

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include "msa2.h"

char ovlboot[] = { 0x0E, 0x1F, 0xBA, 0x0D, 0x00, 0xB4, 0x09, 0xCD, 0x21, 0xB4, 0x4C, 0xCD, 0x21, 0x54, 0x68, 0x69,
0x73, 0x20, 0x69, 0x73, 0x20, 0x6F, 0x76, 0x65, 0x72, 0x6C, 0x61, 0x79, 0x20, 0x66, 0x69, 0x6C,
0x65, 0x2E, 0x0D, 0x0A, 0x24 };

FILE* outfile;
int target;
int outptr;
int voutptr = 0;
byte* outprog;

int errors, warnings;

byte quiet = 1;

int pass;
int passes = 2;

word org = 0x100;
word out_max = 0x1000;

char write_ovl_boot() {
    int i;

    if(target != TARGET_OVL) {
         return 1;
    }

    for(i = 0; i < sizeof(ovlboot); i++) {
         outprog[outptr++] = ovlboot[i];
    }

    return 1;
}

char write_ovl_exports(FILE *o) {
    word magik, count;
    t_constant *c;
    t_ovl_export e;

    if(target != TARGET_OVL && target != TARGET_TEXE) {
         return 1;
    }

    magik = 0xFE33;

    count = 0;

    c = constant;
    while(c != NULL) {
        if(c->export) {
            count++;
        }
        c = c->next;
    }

    fwrite(&magik, 1, sizeof(magik), o);
    fwrite(&count, 1, sizeof(count), o);

    c = constant;
    while(c != NULL) {
        if(c->export) {
            strncpy(e.name, c->name, 8);
            e.flags = 0;
            e.ofs = c->value;
            fwrite(&e, 1, sizeof(e), o);
        }
        c = c->next;
    }
    return 1;
}

char write_exe_header(FILE *o, word entry_point, word image_size, word bss_size) {
    char exe_hdr[0x20];
    int bss_par;
    int block_count;
    int inlastblock;

    bss_par = -(image_size + bss_size);
    if((bss_par & 0x0f) != 0) {
        bss_par = bss_par >> 4 + 1;
    } else {
        bss_par = bss_par >> 4;
    }
    bss_par &= 0x0fff;

    inlastblock = image_size & 0x1ff;
    block_count = (image_size >> 9) & 0x7f;
    if(inlastblock) {
        block_count++;
    }

    memset(exe_hdr, 0, sizeof(exe_hdr));
    exe_hdr[0x00] = 0x4d;
    exe_hdr[0x01] = 0x5a;
    exe_hdr[0x02] = inlastblock & 0xff;
    exe_hdr[0x03] = (inlastblock >> 8) & 0xff;
    exe_hdr[0x04] = block_count & 0xff;
    exe_hdr[0x05] = (block_count >> 8) & 0xff;
    exe_hdr[0x08] = 2;
    exe_hdr[0x0a] = bss_par & 0xff;
    exe_hdr[0x0b] = (bss_par >> 8) & 0xff;
    exe_hdr[0x0c] = bss_par & 0xff;
    exe_hdr[0x0d] = (bss_par >> 8) & 0xff;
    exe_hdr[0x10] = 0xff; /* Default SP */
    exe_hdr[0x11] = 0xfe;
    exe_hdr[0x14] = entry_point & 0xff; /* Entry point IP */
    exe_hdr[0x15] = (entry_point >> 8) & 0xff;

    return fwrite(exe_hdr, 1, sizeof(exe_hdr), o) == sizeof(exe_hdr);
}

void done(int code) {
   t_constant *c;

   while(constant != NULL) {
      c = constant->next;
      free(constant);
      constant = c;
   }

   exit(code);
}

void help(int code) {
        printf( "%s file.asm -ofile.com [-options]\n\n"
                "options:   -sxxxx   set starting point to xxxx (default=0x100)\n"
                "           -bxxxx   set buffer size to xxxx (default=0x1000, 4 KB)\n"
                "           -mx      set error/waning level (default=2)\n"
                "           -px      pass x times (default=2)\n"
                "           -fxxx    set output format (bin,com,texe,ovl)\n\n"
                "Error/Warning levels:\n\n"
                "           0        Errors only\n"
                "           1        Errors and serious warnings\n"
                "           2        All\n\n",PROG_NAME);
         done(code);
}

void main(int argc, char* argv[]) {
        int i;
        char outname[256];
        char *inputname = NULL;

        printf("%s %d.%d (build %d)\nCopyright(C) 2000, 2001, 2019 Robert Ostling\nCopyright(C) 2019 DosWorld\n\n",PROG_NAME,MAIN_VERSION,SUB_VERSION,BUILD);

        if(argc < 2) {
                help(1);
        }
        target = TARGET_UNDEF;
        outname[0] = 0;
        for(i=1;i<argc;i++) {
                if(argv[i][0]=='-') {
                        switch(toupper(argv[i][1])) {
                                case 'F':
                                        if(!stricmp(&argv[i][2],"bin")) {
                                           target = TARGET_BIN;
                                        } else if(!stricmp(&argv[i][2],"com")) {
                                           target = TARGET_COM;
                                        } else if(!stricmp(&argv[i][2],"texe")) {
                                           target = TARGET_TEXE;
                                        } else if(!stricmp(&argv[i][2],"ovl")) {
                                           target = TARGET_OVL;
                                        } else {
                                           help(1);
                                        }
                                        break;
                                case 'O':
                                        strcpy(outname,&argv[i][2]);
                                        break;
                                case 'P':
                                        passes = get_const(&argv[i][2]);
                                        break;
                                case 'S':
                                        org = get_const(&argv[i][2]);
                                        break;
                                case 'B':
                                        out_max = get_const(&argv[i][2]);
                                        break;
                                case 'M':
                                        quiet = get_const(&argv[i][2]);
                                        break;
                                default:
                                        help(1);
                        };
                } else {
                         if(inputname != NULL) {
                                 help(1);
                         }
                         inputname = argv[i];
                }
        }

        if(outname[0] == 0) {
                printf("No output file.\n");
                done(1);
        }

        if((outprog = malloc(out_max)) == NULL) {
                printf("Out of memory.\n");
                done(1);
        }

        if(target == TARGET_UNDEF) {
                target = TARGET_COM;
        }

        switch(target) {
                case TARGET_COM:
                     org = 0x0100; break;
                case TARGET_TEXE:
                case TARGET_OVL:
                     org = 0x0000; break;
        }

        for(pass = 0; pass < passes ;pass++) {
                if((outfile = fopen(outname,"wb"))==0) {
                        printf("Can not open output file: %s.\n",outname);
                        done(1);
                }
                if(target == TARGET_TEXE || target == TARGET_OVL) {
                        write_exe_header(outfile, 0, voutptr + outptr, 0);
                }
                outptr = 0;
                voutptr = 0;
                errors = 0;
                warnings = 0;
                printf("\nPass %d/%d\n", pass+1, passes);
                printf( "Assembling file: %s\n", inputname);
                write_ovl_boot();
                assemble(inputname);
                printf( "%d error%c, %d warning%c\n",errors,(errors==1)?' ':'s',warnings,(warnings==1)?' ':'s');
                fwrite(outprog, outptr, 1, outfile);
                write_ovl_exports(outfile);
                fclose(outfile);
        }

        free(outprog);

        printf("\nAll files assembled.\n");
        if(errors > 0) {
           done(2);
        } else if(warnings > 0) {
           done(1);
        }
        done(0);
}
