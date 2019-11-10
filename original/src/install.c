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


*/

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <alloc.h>
#include <dir.h>

#define BUFF_SIZE       0x1000

void copydir(char* dir, char* todir, char* to) {
        FILE* infile;
        FILE* outfile;
        char tmpdir[0x80];
        char inname[0x80];
        char outname[0x80];
        struct ffblk find;
        int found,tmp,i,read;
        char* copybuff[BUFF_SIZE];

        textattr(0x09);
        cputs("Installing ");
        textattr(0x0f);
        cprintf("%s\n\r",todir);
        sprintf(tmpdir,"%s\\%s",to,todir);
        if(mkdir(tmpdir)) {
                textattr(0x0c);
                cputs("Warning: Can not create directory.\n\r");
        }
        sprintf(inname,"%s\\*.*",todir);
        found=findfirst(inname,&find,1);
        while(found==0) {
                textattr(0x07);
                cprintf("%s...",find.ff_name);
                tmp = 40-strlen(find.ff_name);
                for(i=0;i<tmp;i++) putchar(' ');
                sprintf(outname,"%s\\%s\\%s",to,todir,find.ff_name);
                sprintf(inname,"%s\\%s",dir,find.ff_name);
                if((infile=fopen(inname,"rb"))==NULL) {
                        textattr(0x0c);
                        cputs("Can not open file ");
                        textattr(0x0f);
                        cprintf("%s\n\r",inname);
                        textattr(0x07);
                        exit(1);
                }
                if((outfile=fopen(outname,"wb"))==NULL) {
                        textattr(0x0c);
                        cputs("Can not open/create file ");
                        textattr(0x0f);
                        cprintf("%s\n\r",outname);
                        textattr(0x07);
                        exit(1);
                }
                while(!feof(infile)) {
                        read = fread(copybuff,1,BUFF_SIZE,infile);
                        if(read) fwrite(copybuff,1,read,outfile);
                }
                fclose(infile);
                fclose(outfile);
                textattr(0x0a);
                cputs("OK\n\r");
                found=findnext(&find);
        }
}

void main(void) {
        char todir[0x80];
        char type[10];
        char modautoexec[10];
        int i,j,modae,use8086=0;
        struct ffblk find;
        FILE* autoexec;

        clrscr();
        textattr(0x0c);
        cputs("MSA ");
        textattr(0x0f);
        cputs("1.0.12 installer\n\rCopyright(C) 2001 ");
        textattr(0x09);
        cputs("Robert ™stling\n\r\n\r");
        if(findfirst("install.com",&find,1)) {
                textattr(0x0c);
                cputs("Not in install directory!\n\r");
                textattr(0x07);
                return;
        }
        textattr(0x09);
        cputs("Install to ");
        textattr(0x0f);
        cputs("[c:\\msa]: ");
        gets(todir);
        textattr(0x09);
        cputs("Modify autoexec.bat ");
        textattr(0x0f);
        cputs("[y]: ");
        gets(modautoexec);
        if(!stricmp(modautoexec,"y")||modautoexec[0]==0) modae = 1; else modae = 0;

        textattr(0x09);
        cputs("Install 8086 variant ");
        textattr(0x0f);
        cputs("[n]: ");
        gets(modautoexec);
        if(!stricmp(modautoexec,"y")) use8086 = 1; else modae = 0;

        if(todir[0]==0) {
                strcpy(todir,"c:\\msa");
        }
        if(todir[strlen(todir)-1]=='\\') todir[strlen(todir)-1]=0;
        textattr(0x09);
        cputs(  "Installation type\n\r");
        textattr(0x07);
        cputs(  "1) Minimum    - only msa.com\n\r"
                "2) Normal     - msa.com, example programs and docs\n\r"
                "3) Full       - all abouve + source code\n\r"
                "What type");
        textattr(0x0f);
        cputs(" [2]:");
        gets(type);
        if(type[0]==0) {
                strcpy(type,"2");
        }
        if(type[1]!=0) {
                textattr(0x0c);
                cputs("You can only choose between 1, 2 and 3.\n\r");
                textattr(0x07);
                return;
        }
        switch(type[0]) {
                case '1':
                case '2':
                case '3':
                        break;
                default:
                        textattr(0x0c);
                        cputs("You can only choose between 1, 2 and 3.\n\r");
                        textattr(0x07);
                        return;
        };
        if(mkdir(todir)) {
                textattr(0x0c);
                cputs("Warning: Can not create directory.\n\r");
        }
        if(use8086) copydir("bin8086","bin",todir); else copydir("bin","bin",todir);
        if(type[0]>='2') copydir("examples","examples",todir);
        if(type[0]>='2') copydir("doc","doc",todir);
        if(type[0]=='3') copydir("src","src",todir);

        if(modae) {
                if((autoexec=fopen("c:\\autoexec.bat","a"))==NULL) {
                        textattr(0x0c);
                        cputs("Can not open c:\\autoexec.bat\n\r");
                        textattr(0x07);
                        return;
                }
                fprintf(autoexec,"\nSET PATH=%cPATH%c;%s\\bin\n",'%','%',todir);
                fclose(autoexec);
                textattr(0x0f);
                cputs("After modifying autoexec.bat, you should reboot the computer.\n\r");
        }

        textattr(0x07);
        cputs("Installation complete.\n");
}
