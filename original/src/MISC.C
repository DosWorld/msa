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

void get_arg_types(const char* s) {
    char tmp[64], *p;

    params = 0;
    while(*s) {
        p = tmp;
        while(*s <= ' ' && *s) {
            s++;
        }
        while(*s != ',' && *s) {
            *p = *s;
            p++;
            s++;
        }
        *p = 0;
        while(*p <= ' ' && (p + 1 != tmp)) {
            *p = 0;
            p--;
        }
        strcpy(param[params], tmp);
        param_type[params++] = get_type(tmp);
        if(*s == ',') {
            s++;
        }
    }
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
                    if((!pass && type == CONST_TEXT) || (pass && type == CONST_EXPR)) {
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
    if(c >= 'a' && c <= 'f') return c - 'a' + 10;
    if(c >= 'A' && c <= 'F') return c - 'A' + 10;
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

    while(*s != 0) {
        if(is_math(sign = *s)) {
            s++;
        } else {
            sign = '+';
        }
        j = 0;
        while(is_ident(*s)) {
            tmp[j] = *s;
            j++;
            s++;
        }
        tmp[j] = 0;
        x = 0;
        if(tmp[0] == '\'') {
            x = tmp[1];
        } else if(is_numeric(tmp[0])) {
            x = get_number(tmp);
        } else {
            if((c = find_const(tmp)) != NULL) {
                x = c->value;
            } else {
                if(pass) {
                    sprintf(err_msg, "Undefined constant %s", tmp);
                    out_msg(err_msg, 1);
                }
                return 0x00;
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
        s[strlen(s) - 1]=0;
        a->disp = get_const(s);
        return 0;
    }

    if(is_math(*s)) {
        s[strlen(s) - 1] = 0;
        a->disp = get_const(s);
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

void get_line(char* s) {
    char cf = 0;
    char *os, *p;

    os = s;
    p = prog + ptr;
    while(*p && *p <= ' ') {
        p++;
    }

    while(*p) {
        if(*p == '"') cf ^= 1;
        if(*p == ';'&& !cf) {
            *s = 0;
            break;
        }
        else {
            *s = *p;
            s++;
            p++;
        }
    }
    ptr = p - prog;
    *s = 0;
    os--;
    while(os != s && (*s <= ' ')) {
        *s = 0;
        s--;
    }
}


void split(char* s) {
    char sf = 1, cf = 0, c;
    int i, wp = 0;

    for(i = 0; i < MAX_ARGS; i++) {
        arg[i][0] = 0;
    }

    args = 0;

    while(*s <= ' ' && *s) {
        s++;
    }

    while((c = *s)) {
        if(c == '\"' && cf == 0) {
            cf = 1;
            arg[args][wp++] = c;
        } else if(c == '\"' && cf == 1) {
            arg[args][wp++] = '\"';
            sf = cf = 0;
        } else if(c != '\"' && cf == 1) {
            arg[args][wp++] = c;
        } else if(c <= ' ' && sf == 1) {
        } else if(c != ' '&& sf == 1) {
            sf = 0;
            arg[args][wp++] = toupper(c);
        } else if(c != ' ' && sf == 0) {
            arg[args][wp++] = toupper(c);
        } else if(c <= ' ' && sf == 0) {
            sf = 1;
            arg[args++][wp] = 0;
            wp = 0;
        }
        s++;
    }
    if(sf == 0 || cf == 1) {
        arg[args++][wp] = 0;
    }
}
