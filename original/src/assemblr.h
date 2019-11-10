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

#define MAX_CONSTANT            0x0200
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

typedef struct {
        byte    mod;
        byte    reg;
        byte    rm;
        int     disp;
        byte    op_len;
        byte    op[0x06];
} t_address;

typedef struct {
        char name[20];
        long int value;
} t_constant;

typedef struct {
        char    name[8];
        char    name2[8];
        byte    params;
        byte    param_type[4];
        byte    op[10];
} t_instruction;
