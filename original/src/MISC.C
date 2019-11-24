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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include "MSA2.H"
#include "LEX.H"

#define MSA_MALLOC(c) msa_malloc(c)

t_constant *constants = NULL;

int hashCode(const char *str) {
    int result = 0;

    while(*str) {
        result = result * 31 + *str;
        str++;
    }
    return result;
}


inline char is_ident(char c) {
    return (c>='0' && c<='9') || (c>='a' && c<='z') || (c>='A' && c<='Z') || c=='_' || c=='$' || c=='\'';
}

inline char is_numeric(char c) {
    return c >= '0' && c <= '9';
}

void out_msg(const char* s, int x) {
    if(x == 0) {
        errors++;
    } else {
        warnings++;
    }
    if(quiet >= x) {
        printf("%s:%s:%ld: %s\n", (x == 0 ? "ERROR" : "WARNING"), inputname, linenr, s);
    }
}

void build_address(t_address* a) {
    if(a->mod == 3) {
        a->op_len = 1;
        a->op[0] = (a->mod << 6) + (a->reg << 3) + a->rm;
        return;
    }
    if(a->mod < 3) {
        a->op_len = a->mod + 1;
        if(a->mod == 0 && a->rm == 6) {
            a->op_len = 3;
        }
    }
    a->op[0] = (a->mod << 6) + (a->reg << 3) + a->rm;
    if(a->mod == 1) {
        a->op[1] = a->disp;
    } else if(a->mod == 2 || (a->mod == 0 && a->rm == 6)) {
        a->op[1] = (a->disp) & 0xff;
        a->op[2] = (a->disp >> 8) & 0xff;
    }
}

inline int is_reg(const char *s, const char *k, int badVal) {
    int i;
    char c1, c2;
    char r1, r2;

    c1 = *s;
    s++;
    c2 = *s;
    s++;
    if(*s != 0) {
        return badVal;
    }

    i = 0;
    while((r1 = *k)) {
        k++;
        r2 = *k;
        k++;
        if((c1 == r1) && (c2 == r2)) {
            return i;
        }
        i++;
    }
    return badVal;
}

int is_reg_8(const char* s) {
    return is_reg(s, "ALCLDLBLAHCHDHBH", 8);
}

int is_reg_16(const char* s) {
    return is_reg(s, "AXCXDXBXSPBPSIDI", 8);
}

int is_reg_seg(const char* s) {
    char c1, c2;

    c1 = *s;
    s++;
    c2 = *s;
    s++;
    if((c2 != 'S') || (*s != 0)) {
        return 8;
    }
    switch(c1) {
    case 'E':
        return 0;
    case 'C':
        return 1;
    case 'S':
        return 2;
    case 'D':
        return 3;
    }
    return 8;
}

int get_type(const char* s) {
    int i;

    if((i = is_reg_8(s)) != 8) return ACC_8 + i;
    if((i = is_reg_16(s)) != 8) return ACC_16 + i;
    if((i = is_reg_seg(s)) != 8) return SEG + i;
    if(s[4] == '[') {
        if(!memcmp(s,"BYTE[", 5)) return MEM_8;
        if(!memcmp(s,"WORD[", 5)) return MEM_16;
    }
    if(*s == '[') return MEM_16;
    return IMM;
}

t_constant *add_const(const char* name, int type, long int value) {
    t_constant *c;
    int hash;

    c = constants;
    hash = hashCode(name);

    while(c != NULL) {
        if(hash == c->hash) {
            if(!strcmp(c->name, name)) {
                if(value != c->value) {
                    if((!pass && type == CONST_LABEL) || (pass && type == CONST_EXPR)) {
                        sprintf(err_msg, "Constant %s changed", name);
                        out_msg(err_msg, 2);
                    }
                }
                c->value = value;
                c->type = type;
                return c;
            }
        }
        c = (t_constant *)c->next;
    }
    c = (t_constant *)malloc(sizeof(t_constant));
    strcpy(c->name, name);
    c->value = value;
    c->hash = hash;
    c->type = type;
    c->is_export = 0;
    c->next = constants;
    return constants = c;
}

t_constant *find_const(const char *name) {
    t_constant *c;
    int hash;

    c = constants;
    hash = hashCode(name);
    while(c != NULL) {
        if(hash == c->hash) {
            if(!strcmp(c->name, name)) {
                break;
            }
        }
        c = (t_constant *)c->next;
    }
    return c;
}

inline char is_math(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%';
}

char inline bindigit(char c) {
    if(c == '0' || c == '1') return c - '0';
    sprintf(err_msg, "Invalid binary digit %c", c);
    out_msg(err_msg, 0);
    return 0;
}

char inline decdigit(char c) {
    if(c >= '0' || c <= '9') return c - '0';
    sprintf(err_msg, "Invalid decimal digit %c", c);
    out_msg(err_msg, 0);
    return 0;
}

char inline hexdigit(char c) {
    if(c >= '0' && c <= '9') return c - '0';
    if(c >= 'A' && c <= 'F') return c - 'A' + 10;
    if(c >= 'a' && c <= 'f') return c - 'a' + 10;
    sprintf(err_msg, "Invalid hex digit %c", c);
    out_msg(err_msg, 0);
    return 0;
}

long int get_number(char* s) {
    long int value = 0;
    char c = s[1];

    if(c == 'X' && *s == '0') {
        s += 2;
        while((c = *s)) {
            value = (value << 4) | hexdigit(c);
            s++;
        }
    } else if(c == 'B' && *s == '0') {
        s += 2;
        while((c = *s)) {
            value = (value << 1) | bindigit(c);
            s++;
        }
    } else {
        while((c = *s)) {
            value = (value * 10) + decdigit(c);
            s++;
        }
    }
    return value;
}

long int get_const(const char* s) {
    int j;
    char tmp[32], sign;
    long value, x;
    t_constant *c;

    value = 0;

    while(*s) {
        if(is_math(sign = *s)) {
            s++;
        } else {
            sign = '+';
        }
        if(*s == '\'') {
            s++;
            x = *s;
            s++;
            s++;
        } else {

            j = 0;
            while(is_ident(*s)) {
                tmp[j] = *s;
                j++;
                s++;
            }
            tmp[j] = 0;
            x = 0;
            if(is_numeric(tmp[0])) {
                x = get_number(tmp);
            } else {
                if((c = find_const(tmp)) != NULL) {
                    x = c->value;
                } else {
                    if(pass) {
                        sprintf(err_msg, "Undefined constant '%s'", tmp);
                        out_msg(err_msg, 1);
                    }
                    return 0x00;
                }
            }
        }
        switch(sign) {
        case '+':
            value += x;
            break;
        case '-':
            value -= x;
            break;
        case '*':
            value *= x;
            break;
        case '/':
            value /= x;
            break;
        case '%':
            value = value % x;
            break;
        default:
            sprintf(err_msg, "Unknown math operation %c", sign);
            out_msg(err_msg, 2);
            break;
        }
    }

    return value;
}

int get_address(t_address* a, char* s) {
    int i = 0, k;
    char c1;
    byte seg_pre;

    if((i = is_reg_8(s)) < 8) {
        a->rm = i;
        a->mod = 3;
        return 0;
    } else if((i = is_reg_16(s)) < 8) {
        a->rm = i;
        a->mod = 3;
        return 0;
    }

    while(*s != '[' && *s) {
        s++;
    }

    if(*s == 0) {
        return 1;
    }
    s++;
    i = 0;

    if(s[2] == ':') {
        c1 = *s;
        if(s[1] != 'S') {
            out_msg("Syntax error in segment register name", 0);
            seg_pre = 0x90;
        } else if(c1 == 'C') {
            seg_pre = 0x2e;
        } else if(c1 == 'D') {
            seg_pre = 0x3e;
        } else if(c1 == 'E') {
            seg_pre = 0x26;
        } else if(c1 == 'S') {
            seg_pre = 0x36;
        } else {
            out_msg("Syntax error in segment register name", 0);
            seg_pre = 0x90;
        }
        for(k = old_outptr + 4; k > old_outptr; k--) {
            outprog[k] = outprog[k-1];
        }
        s += 3;
        outprog[old_outptr] = seg_pre;
        outptr++;
    }
    c1 = s[2] == '+';
    if(c1 && !memcmp(s, "BX+SI", 5)) {
        a->rm = 0;
        s += 5;
    } else if(c1 && !memcmp(s, "BX+DI", 5)) {
        a->rm = 1;
        s += 5;
    } else if(c1 && !memcmp(s, "BP+SI", 5)) {
        a->rm = 2;
        s += 5;
    } else if(c1 && !memcmp(s, "BP+DI", 5)) {
        a->rm = 3;
        s += 5;
    } else if(!memcmp(s, "SI", 2)) {
        a->rm = 4;
        s += 2;
    } else if(!memcmp(s, "DI", 2)) {
        a->rm = 5;
        s += 2;
    } else if(!memcmp(s, "BP", 2)) {
        a->rm = 6;
        s += 2;
    } else if(!memcmp(s, "BX", 2)) {
        a->rm = 7;
        s += 2;
    } else {
        a->mod = 0;
        a->rm = 6;
        s[strlen(s) - 1] = 0;
        a->disp = get_const(s);
        return 0;
    }

    if(is_math(*s)) {
        s[strlen(s) - 1] = 0;
        a->disp = get_const(s);
// HERE ??
        a->mod = a->disp < 0x80 ? 1 : 2;
    } else {
        a->mod = 0;
    }

    if(a->rm == 6 && a->mod == 0) {
        a->disp = 0;
        a->mod = 1;
    }

    return 0;
}

inline char is_spc(char c) {
    return c && (c <= ' ');
}

void *msa_malloc(word size) {
    void *r = malloc(size);
    if(r == NULL) {
        printf("ERR: Could not allocate %i byte(s)\n", size);
    }
    return r;
}

char *get_param(char *dest, char *line) {
    char c;
    while((c = *line)) {
        if(c == ',') {
            break;
        } else if(c == ' ') {
            line++;
        } else if(c == '\'' || c == '"') {
            *dest = c;
            dest++;
            line++;
            while((*line) && (*line != c)) {
                *dest = *line;
                dest++;
                line++;
            }
            if(*line) {
                *dest = *line;
                dest++;
                line++;
            }
        } else {
            *dest = c;
            dest++;
            line++;
        }
    }
    *dest = 0;
    return line;
}

void split_line(t_line *cur, char *line, char *a1) {
    char *p, *p2, c, c1, c2, c3, c4, c5;
    char full_param;
    p = line;
    p2 = a1;

    cur->p1[0] = 0;
    cur->p2[0] = 0;
    full_param = 0;
    cur->lex2 = LEX_NONE;
    while((c = *p)) {
        if((c == ' ') || (c == ':')) {
            break;
        }
        *p2 = c;
        p++;
        p2++;
    }
    *p2 = 0;
    while((c = *p) && (c == ' ')) {
        p++;
    }
    if(c == ':') {
        strcpy(cur->label, a1);
        cur->has_label = 1;
        line = p + 1;
    } else {
        c1 = *p;
        c2 = *(p+1);
        c3 = *(p+2);
        c4 = *(p+3);
        if(c1 == 'E' && c2 == 'Q' && c3 == 'U' && c4 == ' ') {
            strcpy(cur->label, a1);
            full_param = 1;
            line = p;
        }
    }
    if(!*line) {
        return;
    }
    while((c = *line) && (c == ' ')) {
        line++;
    }

    c1 = line[0];
    c2 = line[1];
    c3 = line[2];
    c4 = line[3];
    c5 = line[4];

    if((c1 == 'L' && c2 == 'O' && c3 == 'C' && c4 == 'K' && c5 == ' ')) {
        cur->has_lock = 1;
        line += 5;
    }

    c1 = line[0];
    c2 = line[1];
    c3 = line[2];
    c4 = line[3];
    c5 = line[4];

    if((c1 == 'R' && c2 == 'E' && c3 == 'P')) {
        if(c4 == ' ') {
            cur->rep_type = LEX_REP;
            line += 4;
        } else if((c4 == 'Z' || c4 == 'E') && c5 == ' ') {
            cur->rep_type = LEX_REP;
            line += 5;
        } else if((c4 == 'N') && (c5 == 'Z' || c5 == 'E')) {
            cur->rep_type = LEX_REPNZ;
            line += 6;
        }
    }

    p = cur->cmd;
    while((c = toupper(*line)) && (c > ' ')) {
        *p = c;
        line++;
        p++;
    }

    *p = 0;

    while((c = *line) && (c == ' ')) {
        line++;
    }

    if(!*line) {
        return;
    }

    if(!full_param) {
        c1 = cur->cmd[0];
        c2 = cur->cmd[1];
        c3 = cur->cmd[2];
        if((c3 == 0) && (c1 == 'R' || c1 == 'D')) {
            if(c2 == 'B' || c2 == 'W' || c3 == 'D') {
                full_param = 1;
            }
        }
    }

    if(full_param) {
        p = cur->p1;
        while((c = *line)) {
            if(c == ' ') {
                line++;
            } else if(c == '\'' || c == '"') {
                *p = c;
                p++;
                line++;
                while((*line) && (*line != c)) {
                    *p = *line;
                    p++;
                    line++;
                }
                if(*line) {
                    *p = *line;
                    p++;
                    line++;
                }
            } else {
                *p = c;
                p++;
                line++;
            }
        }
        *p = 0;
        cur->pcount = 1;
        return;
    }
    if(!memcmp(line, "SHORT ", 6)) {
        cur->lex2 = LEX_SHORT;
        line += 6;
    } else if(!memcmp(line, "NEAR ", 5)) {
        cur->lex2 = LEX_NEAR;
        line += 5;
    } else if(!memcmp(line, "FAR ", 4)) {
        cur->lex2 = LEX_FAR;
        line += 4;
    }

    line = get_param(a1, line);
    if(*a1 != 0) {
        strcpy(cur->p1, a1);
        cur->pcount++;
    }
    if(*line == ',') {
        line++;
        line = get_param(a1, line);
        strcpy(cur->p2, a1);
        cur->pcount++;
    }
}

char strip_line(char *line) {
    char *line_b, *line_e, *p, *p2, c;
    int error = 0;

    line_b = line;
    while(is_spc(*line_b)) {
        line_b++;
    }
    line_e = line_b;
    while((c = *line_e) && !error) {
        if(c == '\'' || c == '"') {
            line_e++;
            while((*line_e) && (*line_e != c)) {
                line_e++;
            }
            if(*line_e) {
                line_e++;
            } else {
                error = 1;
            }
        } else if(c < ' ') {
            *line_e = ' ';
        } else if(c == ';') {
            *line_e = 0;
            break;
        } else {
            line_e++;
        }
    }
    if(error) {
        return 0;
    }
    p = line_b - 1;
    while(p != line_e) {
        if(*line_e > ' ') {
            break;
        }
        *line_e = 0;
        line_e--;
    }
    p = line;
    p2 = line_b;
    while((c = *p2)) {
        if(c == '"' || c == '\'') {
            *p = *p2;
            p++;
            p2++;
            while((*p2) && (*p2 != c)) {
                *p = *p2;
                p++;
                p2++;
            }
            if(*p2) {
                *p = *p2;
                p++;
                p2++;
            }
        } else if(c == ' ') {
            *p = *p2;
            p++;
            p2++;
            while((c = *p2) && (c == ' ')) {
                p2++;
            }
        } else {
            *p = toupper(*p2);
            p++;
            p2++;
        }
    }
    *p = 0;
    return 1;
}
