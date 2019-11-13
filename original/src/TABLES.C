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
#include <stdlib.h>
#include <stdio.h>
#include "MSA2.H"

t_instruction instruction[instructions] = {
        "aaa","",0,0,0,0,0,OP_CMD_OP,0x37,0,0,0,0,0,0,0,0,
        "aas","",0,0,0,0,0,OP_CMD_OP,0x3f,0,0,0,0,0,0,0,0,
        "aad","",0,0,0,0,0,OP_CMD_OP,0xd5,OP_CMD_OP,0x0a,0,0,0,0,0,0,
        "aad","",1,IMM,0,0,0,OP_CMD_OP,0xd5,OP_CMD_IMM8,0,0,0,0,0,0,0,
        "aam","",0,0,0,0,0,OP_CMD_OP,0xd4,OP_CMD_OP,0x0a,0,0,0,0,0,0,
        "aam","",1,IMM,0,0,0,OP_CMD_OP,0xd4,OP_CMD_IMM8,0,0,0,0,0,0,0,

        "adc","",2,ACC_8,IMM,0,0,OP_CMD_OP,0x14,OP_CMD_IMM8,1,0,0,0,0,0,0,
        "adc","",2,ACC_16,IMM,0,0,OP_CMD_OP,0x15,OP_CMD_IMM16,1,0,0,0,0,0,0,

        "adc","",2,REG_8,RM_8,0,0,OP_CMD_OP,0x12,OP_CMD_RM1_8,1,0,0,0,0,0,0,
        "adc","",2,REG_16,RM_16,0,0,OP_CMD_OP,0x13,OP_CMD_RM1_16,1,0,0,0,0,0,0,
        "adc","",2,RM_8,REG_8,0,0,OP_CMD_OP,0x10,OP_CMD_RM2_8,1,0,0,0,0,0,0,
        "adc","",2,RM_16,REG_16,0,0,OP_CMD_OP,0x11,OP_CMD_RM2_16,1,0,0,0,0,0,0,
        "adc","",2,RM_8,IMM,0,0,OP_CMD_OP,0x80,OP_CMD_RMLINE_8,2,0,OP_CMD_IMM8,1,0,0,0,
        "adc","",2,RM_16,IMM,0,0,OP_CMD_OP,0x81,OP_CMD_RMLINE_16,2,0,OP_CMD_IMM16,1,0,0,0,

        "add","",2,ACC_8,IMM,0,0,OP_CMD_OP,0x04,OP_CMD_IMM8,1,0,0,0,0,0,0,
        "add","",2,ACC_16,IMM,0,0,OP_CMD_OP,0x05,OP_CMD_IMM16,1,0,0,0,0,0,0,

        "add","",2,REG_8,RM_8,0,0,OP_CMD_OP,0x02,OP_CMD_RM1_8,1,0,0,0,0,0,0,
        "add","",2,REG_16,RM_16,0,0,OP_CMD_OP,0x03,OP_CMD_RM1_16,1,0,0,0,0,0,0,
        "add","",2,RM_8,REG_8,0,0,OP_CMD_OP,0x00,OP_CMD_RM2_8,1,0,0,0,0,0,0,
        "add","",2,RM_16,REG_16,0,0,OP_CMD_OP,0x01,OP_CMD_RM2_16,1,0,0,0,0,0,0,
        "add","",2,RM_8,IMM,0,0,OP_CMD_OP,0x80,OP_CMD_RMLINE_8,0,0,OP_CMD_IMM8,1,0,0,0,
        "add","",2,RM_16,IMM,0,0,OP_CMD_OP,0x81,OP_CMD_RMLINE_16,0,0,OP_CMD_IMM16,1,0,0,0,

        "and","",2,ACC_8,IMM,0,0,OP_CMD_OP,0x24,OP_CMD_IMM8,1,0,0,0,0,0,0,
        "and","",2,ACC_16,IMM,0,0,OP_CMD_OP,0x25,OP_CMD_IMM16,1,0,0,0,0,0,0,

        "and","",2,REG_8,RM_8,0,0,OP_CMD_OP,0x22,OP_CMD_RM1_8,1,0,0,0,0,0,0,
        "and","",2,REG_16,RM_16,0,0,OP_CMD_OP,0x23,OP_CMD_RM1_16,1,0,0,0,0,0,0,
        "and","",2,RM_8,REG_8,0,0,OP_CMD_OP,0x20,OP_CMD_RM2_8,1,0,0,0,0,0,0,
        "and","",2,RM_16,REG_16,0,0,OP_CMD_OP,0x21,OP_CMD_RM2_16,1,0,0,0,0,0,0,
        "and","",2,RM_8,IMM,0,0,OP_CMD_OP,0x80,OP_CMD_RMLINE_8,4,0,OP_CMD_IMM8,1,0,0,0,
        "and","",2,RM_16,IMM,0,0,OP_CMD_OP,0x81,OP_CMD_RMLINE_16,4,0,OP_CMD_IMM16,1,0,0,0,

        "bound","",2,REG_16,MEM_16,0,0,OP_CMD_OP,0x62,OP_CMD_RM2_16,1,0,0,0,0,0,0,

        "call","near",1,IMM,0,0,0,OP_CMD_OP,0xe8,OP_CMD_REL16,0,0,0,0,0,0,0,
        "call","far",2,IMM,IMM,0,0,OP_CMD_OP,0x9a,OP_CMD_IMM16,1,OP_CMD_IMM16,0,0,0,0,0,
        "call","",1,IMM,0,0,0,OP_CMD_OP,0xe8,OP_CMD_REL16,0,0,0,0,0,0,0,

        "cbw","",0,0,0,0,0,OP_CMD_OP,0x98,0,0,0,0,0,0,0,0,
        "cwd","",0,0,0,0,0,OP_CMD_OP,0x99,0,0,0,0,0,0,0,0,

        "clc","",0,0,0,0,0,OP_CMD_OP,0xf8,0,0,0,0,0,0,0,0,
        "cld","",0,0,0,0,0,OP_CMD_OP,0xfc,0,0,0,0,0,0,0,0,
        "cli","",0,0,0,0,0,OP_CMD_OP,0xfa,0,0,0,0,0,0,0,0,
        "clts","",0,0,0,0,0,OP_CMD_OP,0x0f,OP_CMD_OP,0x06,0,0,0,0,0,0,

        "cmc","",0,0,0,0,0,OP_CMD_OP,0xf5,0,0,0,0,0,0,0,0,

        "cmp","",2,ACC_8,IMM,0,0,OP_CMD_OP,0x3c,OP_CMD_IMM8,1,0,0,0,0,0,0,
        "cmp","",2,ACC_16,IMM,0,0,OP_CMD_OP,0x3d,OP_CMD_IMM16,1,0,0,0,0,0,0,

        "cmp","",2,RM_8,REG_8,0,0,OP_CMD_OP,0x38,OP_CMD_RM2_8,1,0,0,0,0,0,0,
        "cmp","",2,RM_16,REG_16,0,0,OP_CMD_OP,0x39,OP_CMD_RM2_16,1,0,0,0,0,0,0,
        "cmp","",2,REG_8,RM_8,0,0,OP_CMD_OP,0x3a,OP_CMD_RM1_8,1,0,0,0,0,0,0,
        "cmp","",2,REG_16,RM_16,0,0,OP_CMD_OP,0x3b,OP_CMD_RM1_16,1,0,0,0,0,0,0,
        "cmp","",2,RM_8,IMM,0,0,OP_CMD_OP,0x80,OP_CMD_RMLINE_8,7,0,OP_CMD_IMM8,1,0,0,0,
        "cmp","",2,RM_16,IMM,0,0,OP_CMD_OP,0x81,OP_CMD_RMLINE_16,7,0,OP_CMD_IMM16,1,0,0,0,

        "cmpsb","",0,0,0,0,0,OP_CMD_OP,0xa6,0,0,0,0,0,0,0,0,
        "cmpsw","",0,0,0,0,0,OP_CMD_OP,0xa7,0,0,0,0,0,0,0,0,

        "daa","",0,0,0,0,0,OP_CMD_OP,0x27,0,0,0,0,0,0,0,0,
        "das","",0,0,0,0,0,OP_CMD_OP,0x2f,0,0,0,0,0,0,0,0,

        "dec","",1,REG_16,0,0,0,OP_CMD_PLUSREG16,0x48,0,0,0,0,0,0,0,0,
        "dec","",1,RM_8,0,0,0,OP_CMD_OP,0xfe,OP_CMD_RMLINE_8,1,0,0,0,0,0,0,
        "dec","",1,RM_16,0,0,0,OP_CMD_OP,0xff,OP_CMD_RMLINE_16,1,0,0,0,0,0,0,

        "div","",1,RM_8,0,0,0,OP_CMD_OP,0xf6,OP_CMD_RMLINE_8,6,0,0,0,0,0,0,
        "div","",1,RM_16,0,0,0,OP_CMD_OP,0xf7,OP_CMD_RMLINE_16,6,0,0,0,0,0,0,

        "enter","",2,IMM,IMM,0,0,OP_CMD_OP,0xc8,OP_CMD_IMM16,0,OP_CMD_IMM8,1,0,0,0,0,

        "idiv","",1,RM_8,0,0,0,OP_CMD_OP,0xf6,OP_CMD_RMLINE_8,7,0,0,0,0,0,0,
        "idiv","",1,RM_16,0,0,0,OP_CMD_OP,0xf7,OP_CMD_RMLINE_16,7,0,0,0,0,0,0,

        "imul","",1,RM_8,0,0,0,OP_CMD_OP,0xf6,OP_CMD_RMLINE_8,5,0,0,0,0,0,0,
        "imul","",1,RM_16,0,0,0,OP_CMD_OP,0xf7,OP_CMD_RMLINE_16,5,0,0,0,0,0,0,

        "in","",2,ACC_8,IMM,0,0,OP_CMD_OP,0xe4,OP_CMD_IMM8,1,0,0,0,0,0,0,
        "in","",2,ACC_16,IMM,0,0,OP_CMD_OP,0xe5,OP_CMD_IMM8,1,0,0,0,0,0,0,
        "in","",2,ACC_8,DX,0,0,OP_CMD_OP,0xec,0,0,0,0,0,0,0,0,
        "in","",2,ACC_16,DX,0,0,OP_CMD_OP,0xed,0,0,0,0,0,0,0,0,

        "inc","",1,REG_16,0,0,0,OP_CMD_PLUSREG16,0x40,0,0,0,0,0,0,0,0,
        "inc","",1,RM_8,0,0,0,OP_CMD_OP,0xfe,OP_CMD_RMLINE_8,0,0,0,0,0,0,0,
        "inc","",1,RM_16,0,0,0,OP_CMD_OP,0xff,OP_CMD_RMLINE_16,0,0,0,0,0,0,0,

        "insb","",0,0,0,0,0,OP_CMD_OP,0x6c,0,0,0,0,0,0,0,0,
        "insw","",0,0,0,0,0,OP_CMD_OP,0x6d,0,0,0,0,0,0,0,0,

        "halt","",0,0,0,0,0,OP_CMD_OP,0xf4,0,0,0,0,0,0,0,0,

        "int3","",0,0,0,0,0,OP_CMD_OP,0xcc,0,0,0,0,0,0,0,0,
        "into","",0,0,0,0,0,OP_CMD_OP,0xce,0,0,0,0,0,0,0,0,

        "iret","",0,0,0,0,0,OP_CMD_OP,0xcf,0,0,0,0,0,0,0,0,

        "jcxz","",1,IMM,0,0,0,OP_CMD_OP,0xe3,OP_CMD_REL8,0,0,0,0,0,0,0,

        "ja","",1,IMM,0,0,0,OP_CMD_OP,0x77,OP_CMD_REL8,0,0,0,0,0,0,0,
        "jae","",1,IMM,0,0,0,OP_CMD_OP,0x73,OP_CMD_REL8,0,0,0,0,0,0,0,
        "jb","",1,IMM,0,0,0,OP_CMD_OP,0x72,OP_CMD_REL8,0,0,0,0,0,0,0,
        "jbe","",1,IMM,0,0,0,OP_CMD_OP,0x76,OP_CMD_REL8,0,0,0,0,0,0,0,
        "jc","",1,IMM,0,0,0,OP_CMD_OP,0x72,OP_CMD_REL8,0,0,0,0,0,0,0,
        "je","",1,IMM,0,0,0,OP_CMD_OP,0x74,OP_CMD_REL8,0,0,0,0,0,0,0,
        "jg","",1,IMM,0,0,0,OP_CMD_OP,0x7f,OP_CMD_REL8,0,0,0,0,0,0,0,
        "jge","",1,IMM,0,0,0,OP_CMD_OP,0x7d,OP_CMD_REL8,0,0,0,0,0,0,0,
        "jl","",1,IMM,0,0,0,OP_CMD_OP,0x7c,OP_CMD_REL8,0,0,0,0,0,0,0,
        "jle","",1,IMM,0,0,0,OP_CMD_OP,0x7e,OP_CMD_REL8,0,0,0,0,0,0,0,
        "jna","",1,IMM,0,0,0,OP_CMD_OP,0x76,OP_CMD_REL8,0,0,0,0,0,0,0,
        "jnae","",1,IMM,0,0,0,OP_CMD_OP,0x72,OP_CMD_REL8,0,0,0,0,0,0,0,
        "jnb","",1,IMM,0,0,0,OP_CMD_OP,0x73,OP_CMD_REL8,0,0,0,0,0,0,0,
        "jnbe","",1,IMM,0,0,0,OP_CMD_OP,0x77,OP_CMD_REL8,0,0,0,0,0,0,0,
        "jnc","",1,IMM,0,0,0,OP_CMD_OP,0x73,OP_CMD_REL8,0,0,0,0,0,0,0,
        "jne","",1,IMM,0,0,0,OP_CMD_OP,0x75,OP_CMD_REL8,0,0,0,0,0,0,0,
        "jng","",1,IMM,0,0,0,OP_CMD_OP,0x7e,OP_CMD_REL8,0,0,0,0,0,0,0,
        "jnge","",1,IMM,0,0,0,OP_CMD_OP,0x7c,OP_CMD_REL8,0,0,0,0,0,0,0,
        "jnl","",1,IMM,0,0,0,OP_CMD_OP,0x7d,OP_CMD_REL8,0,0,0,0,0,0,0,
        "jnle","",1,IMM,0,0,0,OP_CMD_OP,0x7f,OP_CMD_REL8,0,0,0,0,0,0,0,
        "jno","",1,IMM,0,0,0,OP_CMD_OP,0x71,OP_CMD_REL8,0,0,0,0,0,0,0,
        "jnp","",1,IMM,0,0,0,OP_CMD_OP,0x7b,OP_CMD_REL8,0,0,0,0,0,0,0,
        "jns","",1,IMM,0,0,0,OP_CMD_OP,0x79,OP_CMD_REL8,0,0,0,0,0,0,0,
        "jnz","",1,IMM,0,0,0,OP_CMD_OP,0x75,OP_CMD_REL8,0,0,0,0,0,0,0,
        "jo","",1,IMM,0,0,0,OP_CMD_OP,0x70,OP_CMD_REL8,0,0,0,0,0,0,0,
        "jp","",1,IMM,0,0,0,OP_CMD_OP,0x7a,OP_CMD_REL8,0,0,0,0,0,0,0,
        "jpe","",1,IMM,0,0,0,OP_CMD_OP,0x7a,OP_CMD_REL8,0,0,0,0,0,0,0,
        "jpo","",1,IMM,0,0,0,OP_CMD_OP,0x7b,OP_CMD_REL8,0,0,0,0,0,0,0,
        "js","",1,IMM,0,0,0,OP_CMD_OP,0x78,OP_CMD_REL8,0,0,0,0,0,0,0,
        "jz","",1,IMM,0,0,0,OP_CMD_OP,0x74,OP_CMD_REL8,0,0,0,0,0,0,0,

        "lahf","",0,0,0,0,0,OP_CMD_OP,0x9f,0,0,0,0,0,0,0,0,

        "lds","",2,REG_16,MEM_16,0,0,OP_CMD_OP,0xc5,OP_CMD_RM1_16,1,0,0,0,0,0,0,
        "les","",2,REG_16,MEM_16,0,0,OP_CMD_OP,0xc4,OP_CMD_RM1_16,1,0,0,0,0,0,0,

        "lea","",2,REG_16,MEM_16,0,0,OP_CMD_OP,0x8d,OP_CMD_RM1_16,1,0,0,0,0,0,0,

        "leave","",0,0,0,0,0,OP_CMD_OP,0xc9,0,0,0,0,0,0,0,0,

        "cmpsb","",0,0,0,0,0,OP_CMD_OP,0xac,0,0,0,0,0,0,0,0,
        "cmpsw","",0,0,0,0,0,OP_CMD_OP,0xad,0,0,0,0,0,0,0,0,

        "loop","",1,IMM,0,0,0,OP_CMD_OP,0xe2,OP_CMD_REL8,0,0,0,0,0,0,0,
        "loope","",1,IMM,0,0,0,OP_CMD_OP,0xe1,OP_CMD_REL8,0,0,0,0,0,0,0,
        "loopne","",1,IMM,0,0,0,OP_CMD_OP,0xe0,OP_CMD_REL8,0,0,0,0,0,0,0,
        "loopz","",1,IMM,0,0,0,OP_CMD_OP,0xe1,OP_CMD_REL8,0,0,0,0,0,0,0,
        "loopnz","",1,IMM,0,0,0,OP_CMD_OP,0xe0,OP_CMD_REL8,0,0,0,0,0,0,0,

        "mov","",2,IMM,ACC_8,0,0,OP_CMD_OP,0xa2,OP_CMD_IMM8,1,0,0,0,0,0,0,
        "mov","",2,IMM,ACC_16,0,0,OP_CMD_OP,0xa3,OP_CMD_IMM16,1,0,0,0,0,0,0,

        "mov","",2,REG_8,IMM,0,0,OP_CMD_PLUSREG8,0xb0,0,OP_CMD_IMM8,1,0,0,0,0,0,
        "mov","",2,REG_16,IMM,0,0,OP_CMD_PLUSREG16,0xb8,0,OP_CMD_IMM16,1,0,0,0,0,0,

        "mov","",2,RM_8,REG_8,0,0,OP_CMD_OP,0x88,OP_CMD_RM2_8,1,0,0,0,0,0,0,
        "mov","",2,RM_16,REG_16,0,0,OP_CMD_OP,0x89,OP_CMD_RM2_16,1,0,0,0,0,0,0,

        "mov","",2,REG_8,RM_8,0,0,OP_CMD_OP,0x8a,OP_CMD_RM1_8,1,0,0,0,0,0,0,
        "mov","",2,REG_16,RM_16,0,0,OP_CMD_OP,0x8b,OP_CMD_RM1_16,1,0,0,0,0,0,0,

        "mov","",2,RM_8,IMM,0,0,OP_CMD_OP,0xc6,OP_CMD_RMLINE_8,0,0,OP_CMD_IMM8,1,0,0,0,
        "mov","",2,RM_16,IMM,0,0,OP_CMD_OP,0xc7,OP_CMD_RMLINE_16,0,0,OP_CMD_IMM16,1,0,0,0,

        "mov","",2,RM_16,REG_SEG,0,0,OP_CMD_OP,0x8c,OP_CMD_RM2_SEG,1,0,0,0,0,0,0,
        "mov","",2,REG_SEG,RM_16,0,0,OP_CMD_OP,0x8e,OP_CMD_RM1_SEG,1,0,0,0,0,0,0,

        "movsb","",0,0,0,0,0,OP_CMD_OP,0xa4,0,0,0,0,0,0,0,0,
        "movsw","",0,0,0,0,0,OP_CMD_OP,0xa5,0,0,0,0,0,0,0,0,

        "mul","",1,RM_8,0,0,0,OP_CMD_OP,0xf6,OP_CMD_RMLINE_8,4,0,0,0,0,0,0,
        "mul","",1,RM_16,0,0,0,OP_CMD_OP,0xf7,OP_CMD_RMLINE_16,4,0,0,0,0,0,0,

        "neg","",1,RM_8,0,0,0,OP_CMD_OP,0xf6,OP_CMD_RMLINE_8,3,0,0,0,0,0,0,
        "neg","",1,RM_16,0,0,0,OP_CMD_OP,0xf7,OP_CMD_RMLINE_8,3,0,0,0,0,0,0,

        "not","",1,RM_8,0,0,0,OP_CMD_OP,0xf6,OP_CMD_RMLINE_8,2,0,0,0,0,0,0,
        "not","",1,RM_16,0,0,0,OP_CMD_OP,0xf7,OP_CMD_RMLINE_8,2,0,0,0,0,0,0,

        "nop","",0,0,0,0,0,OP_CMD_OP,0x90,0,0,0,0,0,0,0,0,

        "or","",2,ACC_8,IMM,0,0,OP_CMD_OP,0x0a,OP_CMD_IMM8,1,0,0,0,0,0,0,
        "or","",2,ACC_16,IMM,0,0,OP_CMD_OP,0x0b,OP_CMD_IMM16,1,0,0,0,0,0,0,

        "or","",2,RM_8,REG_8,0,0,OP_CMD_OP,0x08,OP_CMD_RM2_8,1,0,0,0,0,0,0,
        "or","",2,RM_16,REG_16,0,0,OP_CMD_OP,0x09,OP_CMD_RM2_16,1,0,0,0,0,0,0,
        "or","",2,REG_8,RM_8,0,0,OP_CMD_OP,0x0a,OP_CMD_RM1_8,1,0,0,0,0,0,0,
        "or","",2,REG_16,RM_16,0,0,OP_CMD_OP,0x0b,OP_CMD_RM1_16,1,0,0,0,0,0,0,
        "or","",2,RM_8,IMM,0,0,OP_CMD_OP,0x80,OP_CMD_RMLINE_8,1,0,OP_CMD_IMM8,1,0,0,0,
        "or","",2,RM_16,IMM,0,0,OP_CMD_OP,0x81,OP_CMD_RMLINE_16,1,0,OP_CMD_IMM16,1,0,0,0,

        "out","",2,IMM,ACC_8,0,0,OP_CMD_OP,0xe6,OP_CMD_IMM8,0,0,0,0,0,0,0,
        "out","",2,IMM,ACC_16,0,0,OP_CMD_OP,0xe7,OP_CMD_IMM8,0,0,0,0,0,0,0,
        "out","",2,DX,ACC_8,0,0,OP_CMD_OP,0xee,0,0,0,0,0,0,0,0,
        "out","",2,DX,ACC_16,0,0,OP_CMD_OP,0xef,0,0,0,0,0,0,0,0,

        "outsb","",0,0,0,0,0,OP_CMD_OP,0x6e,0,0,0,0,0,0,0,0,
        "outsw","",0,0,0,0,0,OP_CMD_OP,0x6f,0,0,0,0,0,0,0,0,

        "lodsb","",0,0,0,0,0,OP_CMD_OP,0xac,0,0,0,0,0,0,0,0,
        "lodsw","",0,0,0,0,0,OP_CMD_OP,0xad,0,0,0,0,0,0,0,0,

        "pop","",1,REG_16,0,0,0,OP_CMD_PLUSREG16,0x58,0,0,0,0,0,0,0,0,
        "pop","",1,RM_16,0,0,0,OP_CMD_OP,0x8f,OP_CMD_RMLINE_16,0,0,0,0,0,0,0,

        "pop","",1,DS,0,0,0,OP_CMD_OP,0x1f,0,0,0,0,0,0,0,0,
        "pop","",1,ES,0,0,0,OP_CMD_OP,0x07,0,0,0,0,0,0,0,0,
        "pop","",1,SS,0,0,0,OP_CMD_OP,0x17,0,0,0,0,0,0,0,0,

        "popa","",0,0,0,0,0,OP_CMD_OP,0x61,0,0,0,0,0,0,0,0,
        "popf","",0,0,0,0,0,OP_CMD_OP,0x9d,0,0,0,0,0,0,0,0,

        "push","",1,RM_16,0,0,0,OP_CMD_OP,0xff,OP_CMD_RMLINE_16,6,0,0,0,0,0,0,
        "push","",1,REG_16,0,0,0,OP_CMD_PLUSREG16,0x50,0,0,0,0,0,0,0,0,

        "push","",1,CS,0,0,0,OP_CMD_OP,0x0e,0,0,0,0,0,0,0,0,
        "push","",1,DS,0,0,0,OP_CMD_OP,0x1e,0,0,0,0,0,0,0,0,
        "push","",1,ES,0,0,0,OP_CMD_OP,0x06,0,0,0,0,0,0,0,0,
        "push","",1,SS,0,0,0,OP_CMD_OP,0x16,0,0,0,0,0,0,0,0,

        "push","",1,IMM,0,0,0,OP_CMD_OP,0x6a,OP_CMD_IMM16,0,0,0,0,0,0,0,

        "pusha","",0,0,0,0,0,OP_CMD_OP,0x60,0,0,0,0,0,0,0,0,
        "pushf","",0,0,0,0,0,OP_CMD_OP,0x9c,0,0,0,0,0,0,0,0,

        "rep","",0,0,0,0,0,OP_CMD_OP,0xf3,0,0,0,0,0,0,0,0,
        "repe","",0,0,0,0,0,OP_CMD_OP,0xf2,0,0,0,0,0,0,0,0,
        "repne","",0,0,0,0,0,OP_CMD_OP,0xf3,0,0,0,0,0,0,0,0,
        "repz","",0,0,0,0,0,OP_CMD_OP,0xf2,0,0,0,0,0,0,0,0,
        "repnz","",0,0,0,0,0,OP_CMD_OP,0xf3,0,0,0,0,0,0,0,0,

        "rcl1","",1,RM_8,0,0,0,OP_CMD_OP,0xd0,OP_CMD_RMLINE_8,2,0,0,0,0,0,0,
        "rcl1","",1,RM_16,0,0,0,OP_CMD_OP,0xd1,OP_CMD_RMLINE_16,2,0,0,0,0,0,0,
        "rcl","",2,RM_8,CL,0,0,OP_CMD_OP,0xD2,OP_CMD_RMLINE_8,2,0,0,0,0,0,0,
        "rcl","",2,RM_16,CL,0,0,OP_CMD_OP,0xD3,OP_CMD_RMLINE_16,2,0,0,0,0,0,0,
        "rcl","",2,RM_8,IMM,0,0,OP_CMD_OP,0xc0,OP_CMD_RMLINE_8,2,0,OP_CMD_IMM8,1,0,0,0,
        "rcl","",2,RM_16,IMM,0,0,OP_CMD_OP,0xc1,OP_CMD_RMLINE_16,2,0,OP_CMD_IMM8,1,0,0,0,

        "rcr1","",1,RM_8,0,0,0,OP_CMD_OP,0xd0,OP_CMD_RMLINE_8,3,0,0,0,0,0,0,
        "rcr1","",1,RM_16,0,0,0,OP_CMD_OP,0xd1,OP_CMD_RMLINE_16,3,0,0,0,0,0,0,
        "rcr","",2,RM_8,CL,0,0,OP_CMD_OP,0xD2,OP_CMD_RMLINE_8,3,0,0,0,0,0,0,
        "rcr","",2,RM_16,CL,0,0,OP_CMD_OP,0xD3,OP_CMD_RMLINE_16,3,0,0,0,0,0,0,
        "rcr","",2,RM_8,IMM,0,0,OP_CMD_OP,0xc0,OP_CMD_RMLINE_8,3,0,OP_CMD_IMM8,1,0,0,0,
        "rcr","",2,RM_16,IMM,0,0,OP_CMD_OP,0xc1,OP_CMD_RMLINE_16,3,0,OP_CMD_IMM8,1,0,0,0,

        "rol1","",1,RM_8,0,0,0,OP_CMD_OP,0xd0,OP_CMD_RMLINE_8,0,0,0,0,0,0,0,
        "rol1","",1,RM_16,0,0,0,OP_CMD_OP,0xd1,OP_CMD_RMLINE_16,0,0,0,0,0,0,0,
        "rol","",2,RM_8,CL,0,0,OP_CMD_OP,0xD2,OP_CMD_RMLINE_8,0,0,0,0,0,0,0,
        "rol","",2,RM_16,CL,0,0,OP_CMD_OP,0xD3,OP_CMD_RMLINE_16,0,0,0,0,0,0,0,
        "rol","",2,RM_8,IMM,0,0,OP_CMD_OP,0xc0,OP_CMD_RMLINE_8,0,0,OP_CMD_IMM8,1,0,0,0,
        "rol","",2,RM_16,IMM,0,0,OP_CMD_OP,0xc1,OP_CMD_RMLINE_16,0,0,OP_CMD_IMM8,1,0,0,0,

        "ror1","",1,RM_8,0,0,0,OP_CMD_OP,0xd0,OP_CMD_RMLINE_8,1,0,0,0,0,0,0,
        "ror1","",1,RM_16,0,0,0,OP_CMD_OP,0xd1,OP_CMD_RMLINE_16,1,0,0,0,0,0,0,
        "ror","",2,RM_8,CL,0,0,OP_CMD_OP,0xD2,OP_CMD_RMLINE_8,1,0,0,0,0,0,0,
        "ror","",2,RM_16,CL,0,0,OP_CMD_OP,0xD3,OP_CMD_RMLINE_16,1,0,0,0,0,0,0,
        "ror","",2,RM_8,IMM,0,0,OP_CMD_OP,0xc0,OP_CMD_RMLINE_8,1,0,OP_CMD_IMM8,1,0,0,0,
        "ror","",2,RM_16,IMM,0,0,OP_CMD_OP,0xc1,OP_CMD_RMLINE_16,1,0,OP_CMD_IMM8,1,0,0,0,

        "sahf","",0,0,0,0,0,OP_CMD_OP,0x9e,0,0,0,0,0,0,0,0,

        "sal1","",1,RM_8,0,0,0,OP_CMD_OP,0xd0,OP_CMD_RMLINE_8,4,0,0,0,0,0,0,
        "sal1","",1,RM_16,0,0,0,OP_CMD_OP,0xd1,OP_CMD_RMLINE_16,4,0,0,0,0,0,0,
        "sal","",2,RM_8,CL,0,0,OP_CMD_OP,0xD2,OP_CMD_RMLINE_8,4,0,0,0,0,0,0,
        "sal","",2,RM_16,CL,0,0,OP_CMD_OP,0xD3,OP_CMD_RMLINE_16,4,0,0,0,0,0,0,
        "sal","",2,RM_8,IMM,0,0,OP_CMD_OP,0xc0,OP_CMD_RMLINE_8,4,OP_CMD_IMM8,1,0,0,0,0,
        "sal","",2,RM_16,IMM,0,0,OP_CMD_OP,0xc1,OP_CMD_RMLINE_16,4,OP_CMD_IMM8,1,0,0,0,0,

        "sar1","",1,RM_8,0,0,0,OP_CMD_OP,0xd0,OP_CMD_RMLINE_8,7,0,0,0,0,0,0,
        "sar1","",1,RM_16,0,0,0,OP_CMD_OP,0xd1,OP_CMD_RMLINE_16,7,0,0,0,0,0,0,
        "sar","",2,RM_8,CL,0,0,OP_CMD_OP,0xD2,OP_CMD_RMLINE_8,7,0,0,0,0,0,0,
        "sar","",2,RM_16,CL,0,0,OP_CMD_OP,0xD3,OP_CMD_RMLINE_16,7,0,0,0,0,0,0,
        "sar","",2,RM_8,IMM,0,0,OP_CMD_OP,0xc0,OP_CMD_RMLINE_8,7,0,OP_CMD_IMM8,1,0,0,0,
        "sar","",2,RM_16,IMM,0,0,OP_CMD_OP,0xc1,OP_CMD_RMLINE_16,7,0,OP_CMD_IMM8,1,0,0,0,

        "ret","",0,0,0,0,0,OP_CMD_OP,0xc3,0,0,0,0,0,0,0,0,
        "retf","",0,0,0,0,0,OP_CMD_OP,0xcb,0,0,0,0,0,0,0,0,

        "salc","",0,0,0,0,0,OP_CMD_OP,0xd6,0,0,0,0,0,0,0,0,

        "sbb","",2,ACC_8,IMM,0,0,OP_CMD_OP,0x1c,OP_CMD_IMM8,1,0,0,0,0,0,0,
        "sbb","",2,ACC_16,IMM,0,0,OP_CMD_OP,0x1d,OP_CMD_IMM16,1,0,0,0,0,0,0,

        "sbb","",2,REG_8,RM_8,0,0,OP_CMD_OP,0x1a,OP_CMD_RM1_8,1,0,0,0,0,0,0,
        "sbb","",2,REG_16,RM_16,0,0,OP_CMD_OP,0x1b,OP_CMD_RM1_16,1,0,0,0,0,0,0,
        "sbb","",2,RM_8,REG_8,0,0,OP_CMD_OP,0x18,OP_CMD_RM2_8,1,0,0,0,0,0,0,
        "sbb","",2,RM_16,REG_16,0,0,OP_CMD_OP,0x19,OP_CMD_RM2_16,1,0,0,0,0,0,0,
        "sbb","",2,RM_8,IMM,0,0,OP_CMD_OP,0x80,OP_CMD_RMLINE_8,3,0,OP_CMD_IMM8,1,0,0,0,
        "sbb","",2,RM_16,IMM,0,0,OP_CMD_OP,0x81,OP_CMD_RMLINE_16,3,0,OP_CMD_IMM16,1,0,0,0,

        "scasb","",0,0,0,0,0,OP_CMD_OP,0xae,0,0,0,0,0,0,0,0,
        "scasw","",0,0,0,0,0,OP_CMD_OP,0xaf,0,0,0,0,0,0,0,0,

        "shl1","",1,RM_8,0,0,0,OP_CMD_OP,0xd0,OP_CMD_RMLINE_8,4,0,0,0,0,0,0,
        "shl1","",1,RM_16,0,0,0,OP_CMD_OP,0xd1,OP_CMD_RMLINE_16,4,0,0,0,0,0,0,
        "shl","",2,RM_8,CL,0,0,OP_CMD_OP,0xD2,OP_CMD_RMLINE_8,4,0,0,0,0,0,0,
        "shl","",2,RM_16,CL,0,0,OP_CMD_OP,0xD3,OP_CMD_RMLINE_16,4,0,0,0,0,0,0,
        "shl","",2,RM_8,IMM,0,0,OP_CMD_OP,0xc0,OP_CMD_RMLINE_8,4,0,OP_CMD_IMM8,1,0,0,0,
        "shl","",2,RM_16,IMM,0,0,OP_CMD_OP,0xc1,OP_CMD_RMLINE_16,4,0,OP_CMD_IMM8,1,0,0,0,

        "shr1","",1,RM_8,0,0,0,OP_CMD_OP,0xd0,OP_CMD_RMLINE_8,5,0,0,0,0,0,0,
        "shr1","",1,RM_16,0,0,0,OP_CMD_OP,0xd1,OP_CMD_RMLINE_16,5,0,0,0,0,0,0,
        "shr","",2,RM_8,CL,0,0,OP_CMD_OP,0xD2,OP_CMD_RMLINE_8,5,0,0,0,0,0,0,
        "shr","",2,RM_16,CL,0,0,OP_CMD_OP,0xD3,OP_CMD_RMLINE_16,5,0,0,0,0,0,0,
        "shr","",2,RM_8,IMM,0,0,OP_CMD_OP,0xc0,OP_CMD_RMLINE_8,5,0,OP_CMD_IMM8,1,0,0,0,
        "shr","",2,RM_16,IMM,0,0,OP_CMD_OP,0xc1,OP_CMD_RMLINE_16,5,0,OP_CMD_IMM8,1,0,0,0,

        "stc","",0,0,0,0,0,OP_CMD_OP,0xf9,0,0,0,0,0,0,0,0,
        "std","",0,0,0,0,0,OP_CMD_OP,0xfd,0,0,0,0,0,0,0,0,
        "sti","",0,0,0,0,0,OP_CMD_OP,0xfb,0,0,0,0,0,0,0,0,

        "stosb","",0,0,0,0,0,OP_CMD_OP,0xaa,0,0,0,0,0,0,0,0,
        "stosw","",0,0,0,0,0,OP_CMD_OP,0xab,0,0,0,0,0,0,0,0,

        "sub","",2,ACC_8,IMM,0,0,OP_CMD_OP,0x2c,OP_CMD_IMM8,1,0,0,0,0,0,0,
        "sub","",2,ACC_16,IMM,0,0,OP_CMD_OP,0x2d,OP_CMD_IMM16,1,0,0,0,0,0,0,

        "sub","",2,RM_8,REG_8,0,0,OP_CMD_OP,0x28,OP_CMD_RM2_8,1,0,0,0,0,0,0,
        "sub","",2,RM_16,REG_16,0,0,OP_CMD_OP,0x29,OP_CMD_RM2_16,1,0,0,0,0,0,0,
        "sub","",2,REG_8,RM_8,0,0,OP_CMD_OP,0x2a,OP_CMD_RM1_8,1,0,0,0,0,0,0,
        "sub","",2,REG_16,RM_16,0,0,OP_CMD_OP,0x2b,OP_CMD_RM1_16,1,0,0,0,0,0,0,
        "sub","",2,RM_8,IMM,0,0,OP_CMD_OP,0x80,OP_CMD_RMLINE_8,5,0,OP_CMD_IMM8,1,0,0,0,
        "sub","",2,RM_16,IMM,0,0,OP_CMD_OP,0x81,OP_CMD_RMLINE_16,5,0,OP_CMD_IMM16,1,0,0,0,

        "test","",2,RM_8,REG_8,0,0,OP_CMD_OP,0x84,OP_CMD_RM2_8,1,0,0,0,0,0,0,
        "test","",2,RM_16,REG_16,0,0,OP_CMD_OP,0x85,OP_CMD_RM2_16,1,0,0,0,0,0,0,
        "test","",2,RM_8,IMM,0,0,OP_CMD_OP,0xf6,OP_CMD_RMLINE_8,0,0,OP_CMD_IMM8,1,0,0,0,
        "test","",2,RM_16,IMM,0,0,OP_CMD_OP,0xf7,OP_CMD_RMLINE_16,0,0,OP_CMD_IMM16,1,0,0,0,

        "wait","",0,0,0,0,0,OP_CMD_OP,0x9b,0,0,0,0,0,0,0,0,

        "xchg","",2,ACC_16,REG_16,0,0,OP_CMD_PLUSREG16,0x90,1,0,0,0,0,0,0,0,

        "xchg","",2,RM_8,REG_8,0,0,OP_CMD_OP,0x86,OP_CMD_RM2_8,1,0,0,0,0,0,0,
        "xchg","",2,RM_16,REG_16,0,0,OP_CMD_OP,0x87,OP_CMD_RM2_16,1,0,0,0,0,0,0,
        "xchg","",2,REG_8,RM_8,0,0,OP_CMD_OP,0x86,OP_CMD_RM1_8,1,0,0,0,0,0,0,
        "xchg","",2,REG_16,RM_16,0,0,OP_CMD_OP,0x87,OP_CMD_RM1_16,1,0,0,0,0,0,0,

        "xlatb","",0,0,0,0,0,OP_CMD_OP,0xd7,0,0,0,0,0,0,0,0,

        "xor","",2,ACC_8,IMM,0,0,OP_CMD_OP,0x34,OP_CMD_IMM8,1,0,0,0,0,0,0,
        "xor","",2,ACC_16,IMM,0,0,OP_CMD_OP,0x35,OP_CMD_IMM16,1,0,0,0,0,0,0,

        "xor","",2,RM_8,REG_8,0,0,OP_CMD_OP,0x30,OP_CMD_RM2_8,1,0,0,0,0,0,0,
        "xor","",2,RM_16,REG_16,0,0,OP_CMD_OP,0x31,OP_CMD_RM2_16,1,0,0,0,0,0,0,
        "xor","",2,REG_8,RM_8,0,0,OP_CMD_OP,0x32,OP_CMD_RM1_8,1,0,0,0,0,0,0,
        "xor","",2,REG_16,RM_16,0,0,OP_CMD_OP,0x33,OP_CMD_RM1_16,1,0,0,0,0,0,0,
        "xor","",2,RM_8,IMM,0,0,OP_CMD_OP,0x80,OP_CMD_RMLINE_8,6,0,OP_CMD_IMM8,1,0,0,0,
        "xor","",2,RM_16,IMM,0,0,OP_CMD_OP,0x81,OP_CMD_RMLINE_16,6,0,OP_CMD_IMM16,1,0,0,0,

        "int","",1,IMM,0,0,0,OP_CMD_OP,0xcd,OP_CMD_IMM8,0,0,0,0,0,0,0,

        "jmp","far",2,IMM,IMM,0,0,OP_CMD_OP,0xea,OP_CMD_IMM16,1,OP_CMD_IMM16,0,0,0,0,0,
        "jmp","short",1,IMM,0,0,0,OP_CMD_OP,0xeb,OP_CMD_REL8,0,0,0,0,0,0,0,

        "jmp","",1,IMM,0,0,0,OP_CMD_OP,0xe9,OP_CMD_REL16,0,0,0,0,0,0,0,
        "jmp","near",1,IMM,0,0,0,OP_CMD_OP,0xe9,OP_CMD_REL16,0,0,0,0,0,0,0
};

char* reg_8_table[8] = {
        "AL","CL","DL","BL","AH","CH","DH","BH"
};

char* reg_16_table[8] = {
        "AX","CX","DX","BX","SP","BP","SI","DI"
};

char* reg_seg_table[4] = {
        "ES","CS","SS","DS"
};
