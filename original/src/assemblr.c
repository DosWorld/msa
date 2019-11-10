/*

MIT License

Copyright (c) 2000, 2001, 2019 Robert Östling
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

#include "assemblr.h"
#include "tables.h"

t_constant constant[MAX_CONSTANT];
int constants = 0;
char prog[128];
word ptr;
long int fsize;
long int linenr;
word old_outptr;
char arg[MAX_ARGS][64];
int args;
char param[4][32];
byte param_type[4];
int params;

#include "misc.c"

void out_long(long int x) {
        outprog[outptr++]=x&0xff;
        outprog[outptr++]=(x&0xff00)>>8;
        outprog[outptr++]=(x&0xff0000)>>16;
        outprog[outptr++]=(x&0xff000000)>>24;
}

void out_word(int x) {
        outprog[outptr++]=x&0xff;
        outprog[outptr++]=(x&0xff00)>>8;
}

int assemble(char* fname) {
        FILE* infile;
        char line[128];
        char tmp[64];
        byte cf;
        t_address addr;
        int i,j,k,l,found,first_i,first_i2;
        long int m;
        word w;

        if((infile=fopen(fname,"r"))==0) {
                out_msg("Can not open input file",0);
                return 1;
        }

        linenr = 0;

        while(fgets(prog,128,infile)!=NULL) {
                ptr = 0;
                get_line(line);
                linenr++;
                if(outptr>=out_max-0x80) {
                        voutptr += outptr;
                        if(pass==passes-1) fwrite(outprog,outptr,1,outfile);
                        outptr = 0;
                }

                found=0;
                split(line);
                first_i=0;

                add_const("offset",voutptr+outptr+org);
                add_const("$",voutptr+outptr+org);
                add_const("$$",org);

                if(!stricmp(arg[0],"cs:")) {
                        outprog[outptr++]=0x2e;
                        first_i++;
                } else if(!stricmp(arg[0],"ds:")) {
                        outprog[outptr++]=0x3e;
                        first_i++;
                } else if(!stricmp(arg[0],"es:")) {
                        outprog[outptr++]=0x26;
                        first_i++;
                } else if(!stricmp(arg[0],"ss:")) {
                        outprog[outptr++]=0x36;
                        first_i++;
                } else
                if(arg[0][strlen(arg[0])-1]==':') {
                        arg[0][strlen(arg[0])-1]=0;
                        add_const(arg[0],voutptr+outptr+org);
                        first_i++;
                } else if(!stricmp(arg[0],"rep")||!stricmp(arg[0],"repe")||!stricmp(arg[0],"repz")) {
                        outprog[outptr++]=0xf3;
                        first_i++;
                } else if(!stricmp(arg[0],"repne")||!stricmp(arg[0],"repnz")) {
                        outprog[outptr++]=0xf2;
                        first_i++;
                } else if(!stricmp(arg[1],"db")||!stricmp(arg[1],"dw")||!stricmp(arg[1],"dd")) {
                        add_const(arg[0],voutptr+outptr+org);
                        first_i=1;
                }
                if(!stricmp(arg[first_i],"dd")) {
                        first_i++;
                        found=1;
                        l=0;
                        for(k=0;k<strlen(arg[first_i]);k++) {
                                if(arg[first_i][k]==',') {
                                        tmp[l]=0;
                                        out_long(get_const(tmp));
                                        l=0;
                                } else {
                                        tmp[l++]=arg[first_i][k];
                                }
                        }
                        tmp[l]=0;
                        if(l!=0) {
                                out_long(get_const(tmp));
                        }
                }
                if(!stricmp(arg[first_i],"dw")) {
                        first_i++;
                        l=0;
                        found=1;
                        for(k=0;k<strlen(arg[first_i]);k++) {
                                if(arg[first_i][k]==',') {
                                        tmp[l]=0;
                                        out_word(get_const(tmp));
                                        l=0;
                                } else {
                                        tmp[l++]=arg[first_i][k];
                                }
                        }
                        tmp[l]=0;
                        if(l!=0) {
                                out_word(get_const(tmp));
                        }
                }
                if(!stricmp(arg[first_i],"db")) {
                        first_i++;
                        l=0;
                        cf=0;
                        found=1;
                        for(k=0;k<strlen(arg[first_i]);k++) {
                                if(arg[first_i][k]==','&&!cf) {
                                        tmp[l]=0;
                                        if(l==0) {
                                                k++;
                                                cf=0;
                                        } else {
                                                outprog[outptr++]=get_const(tmp);
                                                k++;
                                        }
                                        if(arg[first_i][k]=='\"') {
                                                cf=1;
                                                l=0;
                                        } else {
                                                cf=0;
                                                l=0;
                                                tmp[l++]=arg[first_i][k];
                                        }
                                } else {
                                        if(arg[first_i][k]=='\"') cf^=1;
                                        else {
                                                if(cf) outprog[outptr++]=arg[first_i][k];
                                                else tmp[l++]=arg[first_i][k];
                                        }
                                }
                        }
                        tmp[l]=0;
                        if(!cf) outprog[outptr++]=get_const(tmp);
                } else
                if(!stricmp(arg[first_i],"str")) {
                        if(arg[first_i+1][0]=='"'&&arg[first_i+1][strlen(arg[first_i+1])-1]=='"') {
                                for(i=1;i<strlen(arg[first_i+1])-1;i++) {
                                        outprog[outptr++]=arg[first_i+1][i];
                                }
                                found=1;
                        }
                        else {
                                out_msg("Syntax error",0);
                        }
                }
                if(!stricmp(arg[first_i],"include")) {
                        add_task(arg[first_i+1]);
                        found=1;
                }
                if(!stricmp(arg[first_i],"org")) {
                        org = get_const(arg[first_i+1]);
                        found=1;
                }
                if(!stricmp(arg[first_i],"const")) {
                        add_const(arg[first_i+1],get_const(arg[first_i+2]));
                        found=1;
                }
                if(!stricmp(arg[first_i+1],"equ")) {
                        add_const(arg[first_i],get_const(arg[first_i+2]));
                        found=1;
                }
                if(args-first_i>3&&!found) {
                        out_msg("Too many parameters",1);
                }
                old_outptr = outptr;
                first_i2=first_i;
                for(i=0;i<instructions&&!found;i++) {
                        first_i=first_i2;
                        if((!strcmp(instruction[i].name2,"")||!stricmp(instruction[i].name2,arg[first_i+1]))&&!stricmp(instruction[i].name,arg[first_i])) {
                                j=1;
                                if(strcmp(instruction[i].name2,"")) {
                                        first_i++;
                                }
                                if(args-first_i>=2) {
                                        get_arg_types(arg[first_i+1]);
                                } else {
                                        params = 0;
                                }
                                if(instruction[i].params!=params) j=0;
                                for(k=0;k<params;k++) {
                                        if(instruction[i].param_type[k]==RM_8) {
                                                if(param_type[k]!=MEM_8&&(param_type[k]<ACC_8||param_type[k]>BH)) {
                                                        j=0;
                                                }
                                        } else
                                        if(instruction[i].param_type[k]==RM_16) {
                                                if(param_type[k]!=MEM_16&&(param_type[k]<ACC_16||param_type[k]>DI)) {
                                                        j=0;
                                                }
                                        } else
                                        if(instruction[i].param_type[k]==REG_8) {
                                                if((param_type[k]<ACC_8||param_type[k]>BH)) {
                                                        j=0;
                                                }
                                        } else
                                        if(instruction[i].param_type[k]==REG_16) {
                                                if((param_type[k]<ACC_16||param_type[k]>DI)) {
                                                        j=0;
                                                }
                                        } else
                                        if(instruction[i].param_type[k]==REG_SEG) {
                                                if((param_type[k]<ES||param_type[k]>DS)) {
                                                        j=0;
                                                }
                                        } else {
                                                if(param_type[k]!=instruction[i].param_type[k]) j=0;
                                        }
                                }
                                if(j==1) {
                                        found=1;
                                        j=0;
                                        while(instruction[i].op[j]!=0) {
                                                switch(instruction[i].op[j]) {
                                                        case OP_CMD_OP:
                                                                j++;
                                                                outprog[outptr++]=instruction[i].op[j];
                                                                break;
                                                        case OP_CMD_IMM8:
                                                                j++;
                                                                outprog[outptr++]=get_const(param[instruction[i].op[j]]);
                                                                break;
                                                        case OP_CMD_IMM16:
                                                                j++;
                                                                outprog[outptr++]=(get_const(param[instruction[i].op[j]]))&0xff;
                                                                outprog[outptr++]=((get_const(param[instruction[i].op[j]]))&0xff00)>>8;
                                                                break;
                                                        case OP_CMD_PLUSREG8:
                                                                j++;
                                                                if(is_reg_8(param[instruction[i].op[j+1]])==8) {
                                                                        out_msg("Syntax error",0);
                                                                }
                                                                outprog[outptr++]=instruction[i].op[j]+is_reg_8(param[instruction[i].op[j+1]]);
                                                                j++;
                                                                break;
                                                        case OP_CMD_PLUSREG16:
                                                                j++;
                                                                if(is_reg_16(param[instruction[i].op[j+1]])==8) {
                                                                        out_msg("Syntax error",0);
                                                                }
                                                                outprog[outptr++]=instruction[i].op[j]+is_reg_16(param[instruction[i].op[j+1]]);
                                                                j++;
                                                                break;
                                                        case OP_CMD_PLUSREGSEG:
                                                                j++;
                                                                if(is_reg_seg(param[instruction[i].op[j+1]])==8) {
                                                                        out_msg("Syntax error",0);
                                                                }
                                                                outprog[outptr++]=instruction[i].op[j]+is_reg_seg(param[instruction[i].op[j+1]]);
                                                                j++;
                                                                break;
                                                        case OP_CMD_RM1_8:
                                                                j++;
                                                                get_address(&addr,param[instruction[i].op[j]]);
                                                                addr.reg = is_reg_8(param[instruction[i].op[j+1]]);
                                                                if(is_reg_8(param[instruction[i].op[j+1]])==8) {
                                                                        out_msg("Syntax error",0);
                                                                }
                                                                build_address(&addr);
                                                                for(k=0;k<addr.op_len;k++) {
                                                                        outprog[outptr++] = addr.op[k];
                                                                }
                                                                j++;
                                                                break;
                                                        case OP_CMD_RM1_16:
                                                                j++;
                                                                get_address(&addr,param[instruction[i].op[j]]);
                                                                addr.reg = is_reg_16(param[instruction[i].op[j+1]]);
                                                                if(is_reg_16(param[instruction[i].op[j+1]])==8) {
                                                                        out_msg("Syntax error",0);
                                                                }
                                                                build_address(&addr);
                                                                for(k=0;k<addr.op_len;k++) {
                                                                        outprog[outptr++] = addr.op[k];
                                                                }
                                                                j++;
                                                                break;
                                                        case OP_CMD_RM2_8:
                                                                j++;
                                                                get_address(&addr,param[instruction[i].op[j+1]]);
                                                                addr.reg = is_reg_8(param[instruction[i].op[j]]);
                                                                if(is_reg_8(param[instruction[i].op[j]])==8) {
                                                                        out_msg("Syntax error",0);
                                                                }
                                                                build_address(&addr);
                                                                for(k=0;k<addr.op_len;k++) {
                                                                        outprog[outptr++] = addr.op[k];
                                                                }
                                                                j++;
                                                                break;
                                                        case OP_CMD_RM2_16:
                                                                j++;
                                                                get_address(&addr,param[instruction[i].op[j+1]]);
                                                                if(is_reg_16(param[instruction[i].op[j]])==8) {
                                                                        out_msg("Syntax error",0);
                                                                }
                                                                addr.reg = is_reg_16(param[instruction[i].op[j]]);
                                                                build_address(&addr);
                                                                for(k=0;k<addr.op_len;k++) {
                                                                        outprog[outptr++] = addr.op[k];
                                                                }
                                                                j++;
                                                                break;
                                                        case OP_CMD_RM2_SEG:
                                                                j++;
                                                                get_address(&addr,param[instruction[i].op[j+1]]);
                                                                addr.reg = is_reg_seg(param[instruction[i].op[j]]);
                                                                if(is_reg_seg(param[instruction[i].op[j]])==8) {
                                                                        out_msg("Syntax error",0);
                                                                }
                                                                build_address(&addr);
                                                                for(k=0;k<addr.op_len;k++) {
                                                                        outprog[outptr++] = addr.op[k];
                                                                }
                                                                j++;
                                                                break;
                                                        case OP_CMD_RM1_SEG:
                                                                j++;
                                                                get_address(&addr,param[instruction[i].op[j]]);
                                                                addr.reg = is_reg_seg(param[instruction[i].op[j+1]]);
                                                                if(is_reg_seg(param[instruction[i].op[j+1]])==8) {
                                                                        out_msg("Syntax error",0);
                                                                }
                                                                build_address(&addr);
                                                                for(k=0;k<addr.op_len;k++) {
                                                                        outprog[outptr++] = addr.op[k];
                                                                }
                                                                j++;
                                                                break;
                                                        case OP_CMD_RMLINE_8:
                                                                j++;
                                                                get_address(&addr,param[instruction[i].op[j+1]]);
                                                                addr.reg = instruction[i].op[j];
                                                                build_address(&addr);
                                                                for(k=0;k<addr.op_len;k++) {
                                                                        outprog[outptr++] = addr.op[k];
                                                                }
                                                                j++;
                                                                break;
                                                        case OP_CMD_RMLINE_16:
                                                                j++;
                                                                get_address(&addr,param[instruction[i].op[j+1]]);
                                                                addr.reg = instruction[i].op[j];
                                                                build_address(&addr);
                                                                for(k=0;k<addr.op_len;k++) {
                                                                        outprog[outptr++] = addr.op[k];
                                                                }
                                                                j++;
                                                                break;
                                                        case OP_CMD_REL8:
                                                                j++;
                                                                if(abs(get_const(param[instruction[i].op[j]])-(outptr+org+1))>127);
                                                                outprog[outptr] = get_const(param[instruction[i].op[j]])-(outptr+org+1);
                                                                outptr++;
                                                                break;
                                                        case OP_CMD_REL16:
                                                                j++;
                                                                w = get_const(param[instruction[i].op[j]])-(outptr+org+2);
                                                                outprog[outptr++] = w&0xff;
                                                                outprog[outptr++] = (w&0xff00)>>8;
                                                                break;
                                                };
                                                j++;
                                        }
                                }
                        }
                }
                if(!found) {
                        out_msg("Syntax error",0);
                }
        }

        fclose(infile);
        return 0;
}
