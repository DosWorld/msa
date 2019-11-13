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

t_constant *constant = NULL;

int hashCode(char *str) {
   int result = 0;

   while(*str) {
      result = result*31 + toupper(*str);
      str++;
   }
   return result;
}

void out_msg(char* s,int x) {
        if(x==0) {
               errors++;
        } else {
               warnings++;
        }
        if(quiet >= x) {
                printf("%06ld: %s\n",linenr,s);
        }
}

void build_address(t_address* a) {
        if(a->mod==3) {
                a->op_len=1;
                a->op[0]=(a->mod << 6) + (a->reg << 3) + a->rm;
                return;
        }
        if(a->mod<3) {
                a->op_len=1+a->mod;
                if(a->mod==0&&a->rm==6) {
                        a->op_len=3;
                }
        }
        a->op[0] = (a->mod << 6) + (a->reg << 3) + a->rm;
        if(a->mod==1) {
                a->op[1] = a->disp;
        } else
        if(a->mod==2||(a->mod==0&&a->rm==6)) {
                a->op[1] = (a->disp) & 0xff;
                a->op[2] = ((a->disp) & 0xff00) >> 8;
        }
}

int is_reg_8(char* s) {
        int i;
        for(i=0;i<8;i++)
                if(!stricmp(s,reg_8_table[i])) return i;
        return 8;
}

int is_reg_16(char* s) {
        int i;
        for(i=0;i<8;i++)
                if(!stricmp(s,reg_16_table[i])) return i;
        return 8;
}

int is_reg_seg(char* s) {
        int i;
        for(i=0;i<4;i++)
                if(!stricmp(s,reg_seg_table[i])) return i;
        return 8;
}

int get_type(char* s) {
        if(is_reg_8(s)!=8) return ACC_8+is_reg_8(s);
        if(is_reg_16(s)!=8) return ACC_16+is_reg_16(s);
        if(is_reg_seg(s)!=8) return SEG+is_reg_seg(s);
        if(!memicmp(s,"BYTE[",5)) return MEM_8;
        if(!memicmp(s,"WORD[",5)) return MEM_16;
        if(!memicmp(s,"[",1)) return MEM_16;
        return IMM;
}

void get_arg_types(char* s) {
        int i=0,j;
        char tmp[64];

        params=0;
        while(s[i]!=0) {
                j = 0;
                while(s[i]!=','&&s[i]!=0) {
                        tmp[j++]=s[i++];
                }
                if(tmp[j-1]==',') tmp[j-1]=0;
                tmp[j]=0;
                strcpy(param[params],tmp);
                param_type[params++]=get_type(tmp);
                if(s[i]==',') i++;
        }
}

void add_const(char* name, long int value) {
        t_constant *c;
        int hash;

        c = constant;
        hash = hashCode(name);
        while(c != NULL) {
                if((hash == c->hash) && !stricmp(c->name,name)) {
                        if(value!=c->value&&stricmp(name,"offset")&&name[0]!='$') {
                                out_msg("Constant changed",2);
                        }
                        c->value = value;
                        return;
                }
                c = c->next;
        }
        c = malloc(sizeof(t_constant));
        strcpy(c->name, name);
        strupr(c->name);
        c->value = value;
        c->hash = hash;
        c->export = 0;
        c->next = constant;
        constant = c;
}

t_constant *find_const(char *name) {
        t_constant *c;
        int hash;

        c = constant;
        hash = hashCode(name);
        while(c != NULL) {
                if((hash == c->hash) && !stricmp(c->name, name)) {
                        break;
                }
                c = c->next;
        }
        return c;
}

int is_ident(char c) {
        if((c>='0'&&c<='9')||(c>='a'&&c<='z')||(c>='A'&&c<='Z')||c=='_'||c=='$'||c=='\'') return 1;
        return 0;
}

int is_numeric(char c) {
        if(c>='0'&&c<='9') return 1;
        return 0;
}

int is_hex_numeric(char c) {
        if((c>='0'&&c<='9')||(c>='a'&&c<='f')||(c>='A'&&c<='F')) return 1;
        return 0;
}

int is_space(char c) {
        if(c==' '||c==0xa||c==0xd||c==0x9) return 1;
        return 0;
}

int hexdigit(char c) {
        if(c >= '0' && c <= '9') return c - '0';
        if(c >= 'a' && c <= 'f') return c - 'a' + 10;
        if(c >= 'A' && c <= 'F') return c - 'A' + 10;
        return 0;
}

long int get_number(char* s) {
        long int value=0;
        int i;

        strupr(s);

        if(s[1]=='X' && s[0]=='0') {
                s += 2;
                while(*s) {
                        if(!is_hex_numeric(*s)) {
                                out_msg("Invalid digit", 0);
                        } else {
                                value = (value << 4) | hexdigit(*s);
                        }
                        s++;
                }
        } else if(s[1] == 'B' && s[0] == '0') {
                s += 2;
                while(*s) {
                        if(*s != '0' && *s!='1') {
                                out_msg("Invalid digit", 0);
                        } else {
                                value = (value << 1) | (*s - '0');
                        }
                        s++;
                }
        } else {
                while(*s) {
                        if(!is_numeric(*s)) {
                                out_msg("Invalid digit", 0);
                        } else {
                                value = (value * 10) + (*s - '0');
                        }
                        s++;
                }
        }
        return value;
}

long int get_const(char* s) {
        int i,j;
        char tmp[32];
        char sign,found;
        long value=0,x;
        t_constant *cnst;

        for(i=0;i<strlen(s);i++) {
                sign = '+';
                switch(s[i]) {
                        case '+':
                        case '-':
                        case '*':
                        case '/':
                        case '%':
                                sign=s[i++]; break;
                        default:
                                break;
                };
                j=0;
                while(is_ident(s[i])) tmp[j++] = s[i++];
                tmp[j]=0;
                i--;
                if(tmp[0]=='\'') {
                        x = tmp[1];
                } else if(is_numeric(tmp[0])) {
                        x = get_number(tmp);
                } else {
                        strupr(tmp);
                        cnst = find_const(tmp);
                        if(cnst == NULL) {
                                out_msg("Unknown constant",2);
                                return 0x1000;
                        } else {
                                x = cnst->value;
                        }
                }
                switch(sign) {
                        case '+':
                                value += x; break;
                        case '-':
                                value -= x; break;
                        case '*':
                                value *= x; break;
                        case '/':
                                value /= x; break;
                        case '%':
                                value = value % x; break;
                        default:
                                break;
                };
        }
        return value;
}

int get_address(t_address* a, char* s) {
        int i=0,j,k;
        byte seg_pre;
        word x;

        if(is_reg_8(s)<8) {
                a->rm = is_reg_8(s);
                a->mod = 3;
                return 0;
        } else
        if(is_reg_16(s)<8) {
                a->rm = is_reg_16(s);
                a->mod = 3;
                return 0;
        }

        while(s[i]!='['&&s[i]!=0) {
                i++;
        }
        if(s[i]==0) return 1;
        i++;
        j=i;

        if(s[i+2]==':') {
                if(!memicmp(&s[i],"CS",2)) seg_pre = 0x2e;
                else if(!memicmp(&s[i],"DS",2)) seg_pre = 0x3e;
                else if(!memicmp(&s[i],"ES",2)) seg_pre = 0x26;
                else if(!memicmp(&s[i],"SS",2)) seg_pre = 0x36;
                else {
                        out_msg("Syntax error",0);
                        seg_pre = 0x90;
                }
                for(k=old_outptr+4;k>old_outptr;k--)
                        outprog[k]=outprog[k-1];
                i+=3;
                j=i;
                outprog[old_outptr]=seg_pre;
                outptr++;
        }

        if(!memicmp(&s[i],"BX+SI",5)) {
                a->rm = 0;
                i += 5;
        } else
        if(!memicmp(&s[i],"BX+DI",5)) {
                a->rm = 1;
                i += 5;
        } else
        if(!memicmp(&s[i],"BP+SI",5)) {
                a->rm = 2;
                i += 5;
        } else
        if(!memicmp(&s[i],"BP+DI",5)) {
                a->rm = 3;
                i += 5;
        } else
        if(!memicmp(&s[i],"SI",2)) {
                a->rm = 4;
                i += 2;
        } else
        if(!memicmp(&s[i],"DI",2)) {
                a->rm = 5;
                i += 2;
        } else
        if(!memicmp(&s[i],"BP",2)) {
                a->rm = 6;
                i += 2;
        } else
        if(!memicmp(&s[i],"BX",2)) {
                a->rm = 7;
                i += 2;
        } else {
                a->mod = 0;
                a->rm = 6;
                s[strlen(s)-1]=0;
                a->disp = get_const(&s[j]);
                return 0;
        }

        if(s[i]=='+'||s[i]=='-'||s[i]=='*'||s[i]=='/'||s[i]=='%') {
                s[strlen(s)-1]=0;
                a->disp = get_const(&s[i]);
                if(a->disp<0x80) a->mod = 1;
                else a->mod = 2;
        } else {
                a->mod = 0;
        }

        if(a->rm==6&&a->mod==0) {
                a->disp = 0;
                a->mod = 1;
        }

        return 0;
}

int get_line(char* s) {
        int i=0;
        byte cf=0;

        while(prog[ptr]!=0x0d&&prog[ptr]!=0x0a&&prog[ptr]!=0) {
                if(prog[ptr]=='"') cf ^= 1;
                if(prog[ptr]==';'&&!cf) {
                        s[i++]=0;
                        ptr++;
                }
                else s[i++]=prog[ptr++];
        }
        s[i]=0;
        if(prog[ptr]==0) return 0;
        ptr++;
        if(prog[ptr]==0x0a||prog[ptr]==0x0d) ptr++;
        if(prog[ptr]==0) return 0;
        return 1;
}

void split(char* s)
{
        byte sf=1,cf=0;
        int i,wp=0;

        for(i=0;i<MAX_ARGS;i++) strcpy(arg[i],"");

        args=0;
        for(i=0;i<strlen(s);i++) {
                if(s[i]=='\"'&&cf==0) {
                        cf=1;
                        arg[args][wp++]=s[i];
                } else
                if(s[i]=='\"'&&cf==1) {
                        arg[args][wp++]='\"';
                        cf=0;
                        sf=0;
                } else
                if(s[i]!='\"'&&cf==1) {
                        arg[args][wp++]=s[i];
                } else
                if(s[i]==' '&&sf==1);
                else
                if(s[i]!=' '&&sf==1) {
                        sf=0;
                        arg[args][wp++]=s[i];
                } else
                if(s[i]!=' '&&sf==0) {
                        arg[args][wp++]=s[i];
                } else
                if(s[i]==' '&&sf==0) {
                        sf=1;
                        arg[args++][wp]=0;
                        wp=0;
                }
        }
        if(sf==0||cf==1) {
                arg[args++][wp]=0;
        }
}
