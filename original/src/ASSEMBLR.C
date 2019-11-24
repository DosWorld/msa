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

long int fsize;
long int linenr;
word old_outptr;
char *param[2];
int param_type[2];
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

char match_params(t_instruction *cinstr, int pcount) {
    int k, type, t;

    for(k = 0; k < pcount; k++) {
        type = param_type[k];
        switch(t = cinstr->param_type[k]) {
        case RM_8:
            if(type !=MEM_8 && (type < ACC_8 || type > BH)) {
                return 0;
            }
            break;
        case RM_16:
            if(type != MEM_16 && (type < ACC_16 || type >= SEG)) {
                return 0;
            }
            break;
        case REG_8:
            if(type < ACC_8 || type >= ACC_16) {
                return 0;
            }
            break;
        case REG_16:
            if((type < ACC_16 || type >= SEG)) {
                return 0;
            }
            break;
        case REG_SEG:
            if((type < SEG || type > DS)) {
                return 0;
            }
            break;
        default:
            if(type != t) {
                return 0;
            }
            break;
        }
    }
    return 1;
}

inline char getReg(int r, int min, int max, const char *msg) {
    if(r < min || r > max) {
        out_msg(msg, 0);
        return 0;
    }
    return r - min;
}

inline void do_instruction(t_instruction *cinstr) {
    t_address addr;
    int k, cf, j = 0;
    int op1, op2, pt1, pt2;
    long z;

    while(cinstr->op[j] != 0) {
        op1 = cinstr->op[j + 1];
        op2 = cinstr->op[j + 2];
        pt1 = param_type[op1];
        pt2 = param_type[op2];
        switch(cinstr->op[j]) {
        case OP_CMD_OP:
            outprog[outptr++] = op1;
            break;
        case OP_CMD_IMM8:
            outprog[outptr++] = get_const(param[op1]);
            break;
        case OP_CMD_IMM16:
            out_word(get_const(param[op1]));
            break;
        case OP_CMD_PLUSREG8:
            outprog[outptr++] = op1 + getReg(pt2, ACC_8, BH, "Syntax error, expected reg8");
            j++;
            break;
        case OP_CMD_PLUSREG16:
            outprog[outptr++] = op1 + getReg(pt2, ACC_16, DI, "Syntax error, expected reg16");
            j++;
            break;
        case OP_CMD_PLUSREGSEG:
            outprog[outptr++] = op1 + getReg(pt2, SEG, DS, "Syntax error, expected segreg");
            j++;
            break;
        case OP_CMD_RM1_8:
            get_address(&addr, param[op1]);
            addr.reg = getReg(pt2, ACC_8, BH, "Syntax error, expected reg8");
            build_address(&addr);
            memcpy(outprog + outptr, &addr.op, addr.op_len);
            outptr += addr.op_len;
            j++;
            break;
        case OP_CMD_RM1_16:
            get_address(&addr, param[op1]);
            addr.reg = getReg(pt2, ACC_16, DI, "Syntax error, expected reg16");
            build_address(&addr);
            memcpy(outprog + outptr, &addr.op, addr.op_len);
            outptr += addr.op_len;
            j++;
            break;
        case OP_CMD_RM2_8:
            get_address(&addr, param[op2]);
            addr.reg = getReg(pt1, ACC_8, BH, "Syntax error, expected reg8");
            build_address(&addr);
            memcpy(outprog + outptr, &addr.op, addr.op_len);
            outptr += addr.op_len;
            j++;
            break;
        case OP_CMD_RM2_16:
            get_address(&addr, param[op2]);
            addr.reg = getReg(pt1, ACC_16, DI, "Syntax error, expected reg16");
            build_address(&addr);
            memcpy(outprog + outptr, &addr.op, addr.op_len);
            outptr += addr.op_len;
            j++;
            break;
        case OP_CMD_RM2_SEG:
            get_address(&addr, param[op2]);
            addr.reg = getReg(pt1, SEG, DS, "Syntax error, expected segreg");
            build_address(&addr);
            memcpy(outprog + outptr, &addr.op, addr.op_len);
            outptr += addr.op_len;
            j++;
            break;
        case OP_CMD_RM1_SEG:
            get_address(&addr, param[op1]);
            addr.reg = getReg(pt2, SEG, DS, "Syntax error, expected segreg");
            build_address(&addr);
            memcpy(outprog + outptr, &addr.op, addr.op_len);
            outptr += addr.op_len;
            j++;
            break;
        case OP_CMD_RMLINE_8:
            get_address(&addr, param[op2]);
            addr.reg = op1;
            build_address(&addr);
            memcpy(outprog + outptr, &addr.op, addr.op_len);
            outptr += addr.op_len;
            j++;
            break;
        case OP_CMD_RMLINE_16:
            get_address(&addr, param[op2]);
            addr.reg = op1;
            build_address(&addr);
            memcpy(outprog + outptr, &addr.op, addr.op_len);
            outptr += addr.op_len;
            j++;
            break;
        case OP_CMD_REL8:
            if(abs(z = get_const(param[op1]) - (outptr + org + 1)) > 127 && pass) {
                out_msg("Too long jump", 1);
            }
            outprog[outptr++] = z & 0xff;
            break;
        case OP_CMD_REL16:
            out_word(get_const(param[op1])-(outptr + org + 2));
            break;
        }
        j += 2;
    }
}

int assemble(char* fname) {
    FILE *infile;
    char *line, *a1, *p;
    t_line *cur;
    char cf, stop, found;
    int j, l, prescan;
    t_constant *org_const, *ofs_const, *c;
    int lex1, lex2;
    t_instruction *cinstr;

    if((infile = fopen(fname,"rb")) == NULL) {
        out_msg("Can't open input file", 0);
        return 0;
    }

    a1 = line = NULL;
    linenr = 0;
    stop = 0;

    ofs_const = find_const("$");
    org_const = find_const("$$");

    cur = (t_line *)malloc(sizeof(t_line));
    line = (char *)malloc(4096);
    a1 = (char *)malloc(4096);
    param[0] = cur->p1;
    param[1] = cur->p2;

    while(fgets(line, 4095, infile) && (!stop)) {

        linenr++;
        strip_line(line);

        memset(cur, 0, sizeof(t_line));
        split_line(cur, line, a1);

        ofs_const->value = voutptr + outptr + org;

        if(outptr >= out_max - 256) {
            voutptr += outptr;
            if(pass == passes - 1) {
                fwrite(outprog, outptr, 1, outfile);
            }
            outptr = 0;
        }


        if(cur->has_label) {
            add_const(cur->label, CONST_LABEL, voutptr + outptr + org);
        }

        if(cur->has_lock) {
            outprog[outptr++] = 0xf0;
        }

        switch(cur->rep_type) {
        case LEX_REP:
            outprog[outptr++] = 0xf3;
            break;
        case LEX_REPNZ:
            outprog[outptr++] = 0xf2;
            break;
        }

        if(cur->cmd[0] == 0) {
            continue;
        }

//        printf("[%li] [%s]:\t[%s]\t[%s],[%s]\n", linenr, cur->label, cur->cmd, cur->p1, cur->p2);

        lex1 = lookupLex(cur->cmd, &prescan);

        switch(lex1) {
        case LEX_DD:
            l = 0;
            j = strlen(cur->p1);
            p = cur->p1;
            while(*p) {
                if(*p ==  ',') {
                    a1[l] = 0;
                    out_long(get_const(a1));
                    l = 0;
                } else {
                    a1[l++] = *p;
                }
                p++;
            }
            a1[l] = 0;
            if(l != 0) {
                out_long(get_const(a1));
            }
            break;
        case LEX_DW:
            l = 0;
            p = cur->p1;
            while(*p) {
                if(*p == ',') {
                    a1[l] = 0;
                    out_word(get_const(a1));
                    l = 0;
                } else {
                    a1[l++] = *p;
                }
                p++;
            }
            a1[l] = 0;
            if(l != 0) {
                out_word(get_const(a1));
            }
            break;
        case LEX_DB:
            p = cur->p1;
            l = 0;
            cf = 0;
            while(*p) {
                if(*p == ',' && !cf) {
                    a1[l] = 0;
                    if(l == 0) {
                        p++;
                        cf = 0;
                    } else {
                        outprog[outptr++] = get_const(a1);
                        p++;
                    }
                    if(*p == '\"') {
                        cf = 1;
                        l = 0;
                    } else {
                        cf = 0;
                        l = 0;
                        a1[l++] = *p;
                    }
                } else {
                    if(*p == '\"') {
                        cf ^= 1;
                    } else {
                        if(cf) {
                            outprog[outptr++] = *p;
                        } else {
                            a1[l++] = *p;
                        }
                    }
                }
                p++;
            }
            a1[l] = 0;
            if(!cf) {
                outprog[outptr++] = get_const(a1);
            }
            break;
        case LEX_EXPORT:
            if((c = find_const(cur->p1)) != NULL) {
                c->is_export = 1;
            }
            break;
        case LEX_ORG:
            org = get_const(cur->p1);
            org_const->value = org;
            break;
        case LEX_END:
            entry_point = get_const(cur->p1);
            entry_point_def = 1;
            stop = 1;
            break;
        case LEX_EQU:
            add_const(cur->label, CONST_EXPR, get_const(cur->p1));
            break;
        case LEX_NONE:
            out_msg("Syntax error", 0);
            stop = 1;
            break;
        default:

            old_outptr = outptr;

            cinstr = &instr86[prescan];

            param_type[0] = cur->pcount > 0 ? get_type(param[0]) : 0;
            param_type[1] = cur->pcount > 1 ? get_type(param[1]) : 0;

            lex2 = cur->lex2;
            found = 0;

            while(cinstr->lex1 != LEX_NONE && !found) {
                if(lex1 != cinstr->lex1) {
                    break;
                }
                if(lex1 != cinstr->lex1 || (cinstr->lex2 != LEX_NONE && lex2 != cinstr->lex2)) {
                    cinstr++;
                    continue;
                }
                if((cur->pcount != cinstr->params) || !match_params(cinstr, cur->pcount)) {
                    cinstr++;
                    continue;
                }
                found = 1;
                do_instruction(cinstr);
                break;
            }
            if(!found) {
                out_msg("Syntax error", 0);
            }
        }
    }
    free(line);
    free(a1);
    fclose(infile);
    return 1;
}
