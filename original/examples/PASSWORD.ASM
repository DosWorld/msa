;
; MIT License
;
; Copyright (c) 2000, 2001, 2019 Robert Ostling
;
; Permission is hereby granted, free of charge, to any person obtaining a copy
; of this software and associated documentation files (the "Software"), to deal
; in the Software without restriction, including without limitation the rights
; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
; copies of the Software, and to permit persons to whom the Software is
; furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in all
; copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
; SOFTWARE.
;


;                               É Password »
;                               ÈÍÍÍÍÍÍÍÍÍÍ¼
;
;                      Copyright(C) 2001 Robert ™stling
;
;       Password is a sample program to the MSA assembler. Can be assembled
;       with both MSA and NASM. Password locks your computer until you
;       unlock it by typing the password. To activate Password, run
;       PASSWORD.COM. Then set the password by pressing <F11>, typing the
;       password, and then press <F11> again. To lock the computer, just
;       press <F12>. To unlock it, type the password.
;
;       Important:      When you are recording a password, _all_ keystrokes
;                       are recorded. If you type SECTER<BKSPACE>T, the key
;                       sequence to unlock is SECTER<BKSPACE>T, not SECRET.

                PUSH    DS
                XOR     AX,AX
                MOV     DS,AX
                MOV     DI,OLD9
                MOV     SI,9*4
                MOVSW
                MOVSW
                MOV     WORD[9*4],NEW9
                MOV     WORD[9*4+2],CS
                POP     DS
                MOV     DX,STR_INSTALLED
                MOV     DX,EOF+1
                INT     0X27

NEW9:           PUSH    AX
                IN      AL,0X60
                TEST    AL,0X80
                JNZ     QUIT9
                CMP     BYTE[CS:PASSREC],1
                JZ      PASS_REC
                CMP     BYTE[CS:WANTPASS],1
                JZ      WANT_PASS
                CMP     AL,0X58
                JZ      F12
                CMP     AL,0X57
                JZ      F11
                JMP     SHORT QUIT9
F11:            MOV     BYTE[CS:PASSREC],1
                MOV     WORD[CS:PASSPOS],0
                JMP     SHORT QUIT9
F12:            MOV     BYTE[CS:WANTPASS],1
                MOV     WORD[CS:PASSCORR],0
                MOV     AL,0XFD
                OUT     0X21,AL
QUIT9:          POP     AX
                DB      0XEA
OLD9:           DD      0

PASS_REC:       CMP     AL,0X57
                JZ      PASS_REC_DONE
                PUSH    SI
                MOV     SI,PASSWORD
                ADD     SI,WORD[CS:PASSPOS]
                MOV     BYTE[CS:SI],AL
                INC     WORD[CS:PASSPOS]
                POP     SI
                JMP     SHORT QUIT9
PASS_REC_DONE:  MOV     BYTE[CS:PASSREC],0
                PUSH    SI
                MOV     SI,PASSWORD
                ADD     SI,WORD[CS:PASSPOS]
                MOV     BYTE[CS:SI],0
                POP     SI
                JMP     SHORT QUIT9

WANT_PASS:      PUSH    SI
                MOV     SI,PASSWORD
                ADD     SI,WORD[CS:PASSCORR]
                CMP     BYTE[CS:SI],AL
                JZ      CHAR_CORRECT
                MOV     WORD[CS:PASSCORR],0
                JMP     SHORT WANT_PASS_DONE
CHAR_CORRECT:
                INC     WORD[CS:PASSCORR]
                CMP     BYTE[CS:SI+1],0
                JNZ     WANT_PASS_DONE
                MOV     BYTE[CS:WANTPASS],0
                XOR     AL,AL
                OUT     0X21,AL
WANT_PASS_DONE:
                POP     SI
                IN      AL,0X61
                MOV     AH,AL
                OR      AL,0X80
                OUT     0X61,AL
                MOV     AL,AH
                OUT     0X61,AL
                MOV     AL,0X20
                OUT     0X20,AL
                POP     AX
                IRET

PASSREC:        DB      0
WANTPASS:       DB      0
PASSPOS:        DW      0
PASSCORR:       DW      0

PASSWORD        EQU     $
EOF             EQU     $+0X20

STR_INSTALLED:  DB      "PASSWORD 1.0",0X0D,0X0A
                DB      "COPYRIGHT(C) 2001 ROBERT ™STLING",0X0D,0X0A,0X0D,0X0A
                DB      "TSR INSTALLED",0X0D,0X0A,"$"
