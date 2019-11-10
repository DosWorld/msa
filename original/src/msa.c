/*

MIT License

Copyright (c) 2000, 2001, 2019 Robert Ã–stling
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

#define MAIN_VERSION    1
#define SUB_VERSION     0
#define BUILD           12
#define PROG_NAME       "MSA"

typedef unsigned char byte;
typedef unsigned int word;
typedef unsigned long int dword;

#include <alloc.h>
#include <stdio.h>

#define MAX_TASK        0x40

FILE* outfile;
int outptr;
int voutptr=0;
byte* outprog;

int errors,warnings;

byte quiet=1;

int pass;
int passes=2;

word org = 0x100;
word out_max = 0x1000;

char task_list[MAX_TASK][64];
int tasks=0;
int task_nr=0;

void add_task(char* fname) {
        int i,found=0;

        for(i=0;i<tasks;i++) {
                if(!stricmp(task_list[i],fname)) {
                        strcpy(task_list[i],fname);
                        found=1;
                }
        }
        if(!found) {
                if(tasks<MAX_TASK) {
                        strcpy(task_list[tasks++],fname);
                } else {
                        printf("Too much to do.\n");
                        exit(1);
                }
        }
}

#include "assemblr.c"

void help(void) {
        printf( "%s file.asm [file2.asm ...] -ofile.com [-options]\n\n"
                "options:   -sxxxx   set starting point to xxxx (default=0x100)\n"
                "           -bxxxx   set buffer size to xxxx (default=0x1000, 4 KB)\n"
                "           -mx      set error/waning level (default=2)\n"
                "           -px      pass x times (default=2)\n\n"
                "Error/Warning levels:\n\n"
                "           0        Errors only\n"
                "           1        Errors and serious warnings\n"
                "           2        All\n\n",PROG_NAME);
}

int main(int argc, char* argv[]) {
        int i;
        char outname[80];

        printf("%s %d.%d (build %d) Copyright(C) 2001 Robert ™stling\n\n",PROG_NAME,MAIN_VERSION,SUB_VERSION,BUILD);

        if(argc<2) {
                help();
                return 1;
        }
        outname[0] = 0;
        for(i=1;i<argc;i++) {
                if(argv[i][0]=='-') {
                        switch(argv[i][1]) {
                                case 'o':
                                case 'O':
                                        strcpy(outname,&argv[i][2]);
                                        break;
                                case 'p':
                                case 'P':
                                        passes = get_const(&argv[i][2]);
                                        break;
                                case 's':
                                case 'S':
                                        org = get_const(&argv[i][2]);
                                        break;
                                case 'b':
                                case 'B':
                                        out_max = get_const(&argv[i][2]);
                                        break;
                                case 'm':
                                case 'M':
                                        quiet = get_const(&argv[i][2]);
                                        break;
                                default:
                                        help();
                                        return 1;
                        };
                } else {
                        add_task(argv[i]);
                }
        }

        if(outname[0]==0) {
                printf("No output file.\n");
                return 2;
        }

        if((outprog=malloc(out_max))==0) {
                printf("Out of memory.\n");
                return 1;
        }

        for(pass=0;pass<passes;pass++) {
                if((outfile=fopen(outname,"wb"))==0) {
                        printf("Can not open output file: %s.\n",outname);
                        return 3;
                }
                outptr=0;
                voutptr=0;
                errors=0;
                warnings=0;
                printf("\nPass %d/%d\n",pass+1,passes);
                for(i=0;i<tasks;i++) {
                        printf( "Assembling file: %s\n",task_list[i]);
                        assemble(task_list[i]);
                        printf( "%d error%c, %d warning%c\n",errors,(errors==1)?' ':'s',warnings,(warnings==1)?' ':'s');
                }
                fwrite(outprog,outptr,1,outfile);
                fclose(outfile);
        }

        free(outprog);

        printf("\nAll files assembled.\n");
        if(errors>0) return 2;
        else if(warnings>0) return 1;
        return 0;
}
