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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MSA2.H"
#include "LEX.H"

char prog[256];
word ptr;

long int fsize;
long int linenr;
char arg[MAX_ARGS][256];
int args;
word old_outptr;
char param[4][32];
byte param_type[4];
int params;

inline void out_word(int x) {
    outprog[outptr++] = x & 0xff;
    outprog[outptr++] = (char)(x >> 8) & 0xff;
}

inline void out_long(long int x) {
    outprog[outptr++] = x & 0xff;
    outprog[outptr++] = (char)(x >> 8) & 0xff;
    outprog[outptr++] = (char)(x >> 16) & 0xff;
    outprog[outptr++] = (char)(x >> 24) & 0xff;
}

char inpbuf[1024*8];
int inpbufPtr;
int inpbufCount;

char ffgets(char *p, int size, FILE *f) {
    char c;
    int readed = 0;

    while(size != 0) {
        while(size != 0 && inpbufPtr != inpbufCount) {
            switch(c = inpbuf[inpbufPtr++]) {
            case 0x0d:
                break;
            case 0x0a:
                *p = 0;
                return 1;
            default:
                *p = c;
                p++;
                size--;
            }
            readed++;
        }
        if(inpbufPtr == inpbufCount) {
            inpbufPtr = 0;
            inpbufCount = fread(inpbuf, 1, sizeof(inpbuf), f);
            if(inpbufCount == 0) {
                *p = 0;
                return readed != 0;
            }
        }
    }
    *p = 0;
    return 1;
}

int assemble(char* fname) {
    FILE* infile;
    char line[256];
    char tmp[64];
    char cf, stop, found;
    t_address addr;
    int j, k, l, z, first_i, first_i2;
    t_constant *org_const, *ofs_const, *c;
    int lex1, lex2;
    t_instruction *cinstr;

    if((infile = fopen(fname,"rb")) == NULL) {
        out_msg("Can't open input file", 0);
        return 0;
    }

    linenr = 0;
    stop = 0;

    ofs_const = find_const("$");
    org_const = find_const("$$");

    inpbufCount = inpbufPtr = 0;
    while(fgets(prog, 256, infile) && (!stop)) {
        ptr = 0;
        get_line(line);
        ofs_const->value = voutptr + outptr + org;
// printf("%04lx: %s\n", ofs_const->value, line);
        linenr++;
        if(outptr >= out_max - 256) {
            voutptr += outptr;
            if(pass == passes - 1) {
                fwrite(outprog, outptr, 1, outfile);
            }
            outptr = 0;
        }

        found = 0;
        split(line);
        first_i = 0;

        k = strlen(arg[first_i]);
        lex1 = lookupLex(arg[first_i]);

        if(arg[first_i][k-1] == ':') {
            switch(lex1) {
            case LEX_CS2DOT:
                outprog[outptr++] = 0x2e;
                first_i++;
                lex1 = lookupLex(arg[first_i]);
                break;
            case LEX_DS2DOT:
                outprog[outptr++] = 0x3e;
                first_i++;
                lex1 = lookupLex(arg[first_i]);
                break;
            case LEX_ES2DOT:
                outprog[outptr++] = 0x26;
                first_i++;
                lex1 = lookupLex(arg[first_i]);
                break;
            case LEX_SS2DOT:
                outprog[outptr++] = 0x36;
                first_i++;
                lex1 = lookupLex(arg[first_i]);
                break;
            default:
                arg[0][k-1] = 0;
                add_const(arg[0], CONST_TEXT, voutptr + outptr + org);
                first_i++;
                lex1 = lookupLex(arg[first_i]);
            }
        }

        switch(lex1) {
        case LEX_REP:
            outprog[outptr++] = 0xf3;
            first_i++;
            lex1 = lookupLex(arg[first_i]);
            break;
        case LEX_REPNZ:
            outprog[outptr++] = 0xf2;
            first_i++;
            lex1 = lookupLex(arg[first_i]);
            break;
        case LEX_DD:
            first_i++;
            l = 0;
            j = strlen(arg[first_i]);
            for(k = 0; k < j; k++) {
                if(arg[first_i][k] ==  ',') {
                    tmp[l] = 0;
                    out_long(get_const(tmp));
                    l = 0;
                } else if(arg[first_i][k] > ' ') {
                    tmp[l++] = arg[first_i][k];
                }
            }
            tmp[l] = 0;
            if(l != 0) {
                out_long(get_const(tmp));
            }
            continue;
        case LEX_DW:
            first_i++;
            l = 0;
            j = strlen(arg[first_i]);
            for(k = 0; k < j; k++) {
                if(arg[first_i][k] == ',') {
                    tmp[l] = 0;
                    out_word(get_const(tmp));
                    l = 0;
                } else if(arg[first_i][k] > ' ') {
                    tmp[l++] = arg[first_i][k];
                }
            }
            tmp[l] = 0;
            if(l != 0) {
                out_word(get_const(tmp));
            }
            continue;
        case LEX_DB:
            first_i++;
            l = 0;
            cf = 0;
            j = strlen(arg[first_i]);
            for(k = 0; k < j; k++) {
                if(arg[first_i][k] == ',' && !cf) {
                    tmp[l] = 0;
                    if(l == 0) {
                        k++;
                        cf = 0;
                    } else {
                        outprog[outptr++] = get_const(tmp);
                        k++;
                    }
                    if(arg[first_i][k] == '\"') {
                        cf = 1;
                        l = 0;
                    } else {
                        cf = 0;
                        l = 0;
                        tmp[l++] = arg[first_i][k];
                    }
                } else {
                    if(arg[first_i][k] == '\"') {
                        cf ^= 1;
                    } else {
                        if(cf) {
                            outprog[outptr++] = arg[first_i][k];
                        } else {
                            tmp[l++] = arg[first_i][k];
                        }
                    }
                }
            }
            tmp[l] = 0;
            if(!cf) {
                outprog[outptr++] = get_const(tmp);
            }
            continue;
        case LEX_EXPORT:
            first_i++;
            if((c = find_const(arg[first_i])) != NULL) {
                c->is_export = 1;
            }
            continue;
        case LEX_ORG:
            org = get_const(arg[first_i+1]);
            org_const->value = org;
            found = 1;
            continue;
        case LEX_END:
            entry_point = get_const(arg[first_i+1]);
            entry_point_def = 1;
            found = 1;
            stop = 1;
            break;
        case LEX_CONST:
            add_const(arg[first_i+1], CONST_EXPR, get_const(arg[first_i+2]));
            continue;
        }

        if(lex1 == LEX_NONE && !strcasecmp(arg[first_i+1], "equ")) {
            add_const(arg[first_i], CONST_EXPR, get_const(arg[first_i+2]));
            continue;
        }

        if(args-first_i > 3 && !found) {
            out_msg("Too many parameters", 1);
            return 0;
        }

        old_outptr = outptr;

        if(found || !arg[first_i][0]) {
            continue;
        }

        lex2 = lookupLex(arg[first_i + 1]);
        first_i2 = first_i;
        cinstr = instructions;

        while(cinstr->lex1 != LEX_NONE && !found) {
            first_i = first_i2;
            if(lex1 != cinstr->lex1 || (cinstr->lex2 != LEX_NONE && lex2 != cinstr->lex2)) {
                cinstr++;
                continue;
            }
            j = 1;

            if(cinstr->lex2 != LEX_NONE) {
                first_i++;
            }

            if(args - first_i >= 2) {
                get_arg_types(arg[first_i+1]);
            } else {
                params = 0;
            }

            if(cinstr->params == params) {
            for(k = 0; k < params && j; k++) {
                switch(cinstr->param_type[k]) {
                case RM_8:
                    if(param_type[k] !=MEM_8 && (param_type[k] < ACC_8 || param_type[k] > BH)) {
                        j = 0;
                    }
                    break;
                case RM_16:
                    if(param_type[k] != MEM_16 && (param_type[k] < ACC_16 || param_type[k] > DI)) {
                        j = 0;
                    }
                    break;
                case REG_8:
                    if((param_type[k] < ACC_8 || param_type[k] > BH)) {
                        j = 0;
                    }
                    break;
                case REG_16:
                    if((param_type[k] < ACC_16 || param_type[k] > DI)) {
                        j = 0;
                    }
                    break;
                case REG_SEG:
                    if((param_type[k] < ES || param_type[k] > DS)) {
                        j =0;
                    }
                    break;
                default:
                    if(param_type[k] != cinstr->param_type[k]) {
                        j = 0;
                    }
                    break;
                }
            }
            } else {
              j = 0;
            }
            if(j == 1) {
                found = 1;
                j = 0;
                while(cinstr->op[j] != 0) {
                    switch(cinstr->op[j]) {
                    case OP_CMD_OP:
                        j++;
                        outprog[outptr++] = cinstr->op[j];
                        break;
                    case OP_CMD_IMM8:
                        j++;
                        outprog[outptr++] = get_const(param[cinstr->op[j]]);
                        break;
                    case OP_CMD_IMM16:
                        j++;
                        out_word(get_const(param[cinstr->op[j]]));
                        break;
                    case OP_CMD_PLUSREG8:
                        j++;
                        if((cf = is_reg_8(param[cinstr->op[j + 1]])) == 8) {
                            out_msg("Syntax error, expected reg8", 0);
                        }
                        outprog[outptr++] = cinstr->op[j] + cf;
                        j++;
                        break;
                    case OP_CMD_PLUSREG16:
                        j++;
                        if((cf = is_reg_16(param[cinstr->op[j + 1]])) == 8) {
                            out_msg("Syntax error, expected reg16", 0);
                        }
                        outprog[outptr++] = cinstr->op[j] + cf;
                        j++;
                        break;
                    case OP_CMD_PLUSREGSEG:
                        j++;
                        if((cf = is_reg_seg(param[cinstr->op[j + 1]])) == 8) {
                            out_msg("Syntax error, expected segreg", 0);
                        }
                        outprog[outptr++] = cinstr->op[j] + cf;
                        j++;
                        break;
                    case OP_CMD_RM1_8:
                        j++;
                        get_address(&addr, param[cinstr->op[j]]);
                        if((addr.reg = is_reg_8(param[cinstr->op[j + 1]])) == 8) {
                            out_msg("Syntax error, expected reg8", 0);
                        }
                        build_address(&addr);
                        k = 0;
                        while(k < addr.op_len) {
                            outprog[outptr++] = addr.op[k++];
                        }
                        j++;
                        break;
                    case OP_CMD_RM1_16:
                        j++;
                        get_address(&addr,param[cinstr->op[j]]);
                        if((addr.reg = is_reg_16(param[cinstr->op[j + 1]])) == 8) {
                            out_msg("Syntax error, expected reg16", 0);
                        }
                        build_address(&addr);
                        k = 0;
                        while(k < addr.op_len) {
                            outprog[outptr++] = addr.op[k++];
                        }
                        j++;
                        break;
                    case OP_CMD_RM2_8:
                        j++;
                        get_address(&addr, param[cinstr->op[j + 1]]);
                        if((addr.reg = is_reg_8(param[cinstr->op[j]])) == 8) {
                            out_msg("Syntax error, expected reg8",0);
                        }
                        build_address(&addr);
                        k = 0;
                        while(k < addr.op_len) {
                            outprog[outptr++] = addr.op[k++];
                        }
                        j++;
                        break;
                    case OP_CMD_RM2_16:
                        j++;
                        get_address(&addr, param[cinstr->op[j + 1]]);
                        if((addr.reg = is_reg_16(param[cinstr->op[j]])) == 8) {
                            out_msg("Syntax error, expected reg16", 0);
                        }
                        build_address(&addr);
                        k = 0;
                        while(k < addr.op_len) {
                            outprog[outptr++] = addr.op[k++];
                        }
                        j++;
                        break;
                    case OP_CMD_RM2_SEG:
                        j++;
                        get_address(&addr, param[cinstr->op[j + 1]]);
                        if((addr.reg = is_reg_seg(param[cinstr->op[j]])) == 8) {
                            out_msg("Syntax error, expected segreg", 0);
                        }
                        build_address(&addr);
                        k = 0;
                        while(k < addr.op_len) {
                            outprog[outptr++] = addr.op[k++];
                        }
                        j++;
                        break;
                    case OP_CMD_RM1_SEG:
                        j++;
                        get_address(&addr, param[cinstr->op[j]]);
                        if((addr.reg = is_reg_seg(param[cinstr->op[j + 1]])) == 8) {
                            out_msg("Syntax error, expected segreg", 0);
                        }
                        build_address(&addr);
                        k = 0;
                        while(k < addr.op_len) {
                            outprog[outptr++] = addr.op[k++];
                        }
                        j++;
                        break;
                    case OP_CMD_RMLINE_8:
                        j++;
                        get_address(&addr, param[cinstr->op[j + 1]]);
                        addr.reg = cinstr->op[j];
                        build_address(&addr);
                        k = 0;
                        while(k < addr.op_len) {
                            outprog[outptr++] = addr.op[k++];
                        }
                        j++;
                        break;
                    case OP_CMD_RMLINE_16:
                        j++;
                        get_address(&addr, param[cinstr->op[j + 1]]);
                        addr.reg = cinstr->op[j];
                        build_address(&addr);
                        k = 0;
                        while(k < addr.op_len) {
                            outprog[outptr++] = addr.op[k++];
                        }
                        j++;
                        break;
                    case OP_CMD_REL8:
                        j++;
                        if(abs(z = get_const(param[cinstr->op[j]]) - (outptr + org + 1)) > 127 && pass) {
                            out_msg("Too long jump", 1);
                        }
                        outprog[outptr] = z & 0xff;
                        outptr++;
                        break;
                    case OP_CMD_REL16:
                        j++;
                        out_word(get_const(param[cinstr->op[j]])-(outptr + org + 2));
                        break;
                    };
                    j++;
                }
            }
            cinstr++;
        }
        if(!found) {
            out_msg("Syntax error", 0);
        }
    }

    fclose(infile);
    return 1;
}
