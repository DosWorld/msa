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

#ifndef _MSA2_H_
#define _MSA2_H_

#define MAIN_VERSION    1
#define SUB_VERSION     0
#define BUILD           14
#define PROG_NAME       "MSA2"

#define TARGET_UNDEF 0
#define TARGET_BIN 1
#define TARGET_COM 2
#define TARGET_TEXE 3
#define TARGET_OVL 100

#define EXPORT_NAME_LENGTH 32

typedef uint8_t byte;
typedef uint16_t word;
typedef uint32_t dword;

#define MAX_ARGS                0x0006

#define IMM                     0X00
#define REG_8                   0X01
#define REG_16                  0X02
#define REG_SEG                 0X03
#define MEM_8                   0X04
#define MEM_16                  0X05
#define RM_8                    0X06
#define RM_16                   0X07
#define ACC_8                   0X10
#define AL                      0X10
#define CL                      0X11
#define DL                      0X12
#define BL                      0X13
#define AH                      0X14
#define CH                      0X15
#define DH                      0X16
#define BH                      0X17
#define ACC_16                  0X20
#define AX                      0X20
#define CX                      0X21
#define DX                      0X22
#define BX                      0X23
#define SP                      0X24
#define BP                      0X25
#define SI                      0X26
#define DI                      0X27
#define SEG                     0X30
#define ES                      0X30
#define CS                      0X31
#define SS                      0X32
#define DS                      0X33

#define OP_CMD_OP               0X01
#define OP_CMD_IMM8             0X02
#define OP_CMD_IMM16            0X03
#define OP_CMD_PLUSREG8         0X04
#define OP_CMD_PLUSREG16        0X05
#define OP_CMD_PLUSREGSEG       0X06
#define OP_CMD_RM1_8            0X07
#define OP_CMD_RM1_16           0X08
#define OP_CMD_RM2_8            0X09
#define OP_CMD_RM2_16           0X0A
#define OP_CMD_RMLINE_8         0X0B
#define OP_CMD_RMLINE_16        0X0C
#define OP_CMD_REL8             0X0D
#define OP_CMD_REL16            0X0E
#define OP_CMD_RM1_SEG          0X0F
#define OP_CMD_RM2_SEG          0X10

#define OVL_SYMTAB_MAGIK        0xFE33

#define CONST_TEXT 1
#define CONST_DATA 2
#define CONST_BSS 3
#define CONST_EXPR 4

typedef struct {
    byte    mod;
    byte    reg;
    byte    rm;
    int     disp;
    byte    op_len;
    byte    op[0x06];
} t_address;

typedef struct {
    char name[64];
    char is_export;
    int hash;
    char type;
    long int value;
    void *next;
} t_constant;

typedef struct {
    int    lex1, lex2;
    int    params;
    int    param_type[4];
    int    op[10];
} t_instruction;

#pragma pack(push)
#pragma pack(1)
typedef struct {
    char  name[EXPORT_NAME_LENGTH];
    word  ofs;
    word  flags;
} t_ovl_export;
#pragma pack(pop)

extern t_instruction instructions[];

extern int assemble(char* fname);
extern void out_msg(const char* s,int x);
extern void build_address(t_address* a);

extern int is_reg_8(const char* s);
extern int is_reg_16(const char* s);
extern int is_reg_seg(const char* s);

extern int get_type(const char* s);
extern void get_arg_types(const char* s);
extern int hashCode(const char *str);
extern long int get_number(char* s);
extern int get_address(t_address* a, char* s);
extern void get_line(char* s);
extern void split(char* s);

extern char err_msg[];
extern int errors, warnings, outptr, voutptr, pass, passes, params;
extern byte quiet;
extern byte *outprog, param_type[4];
extern word out_max, org;
extern FILE *outfile;
extern long int linenr;
extern char param[4][32];
extern word old_outptr;
extern char prog[];
extern word ptr;
extern char arg[MAX_ARGS][256];
extern int args;

extern t_constant *constants;
extern t_constant *add_const(const char* name, int type, long int value);
extern long int get_const(const char* s);
extern t_constant *find_const(const char *name);

extern word entry_point;
extern char entry_point_def;
extern char *inputname;

#endif
