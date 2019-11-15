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
                ORG     0X100

                MOV     AH,0X4A
                MOV     BX,4096
                INT     0X21
                JNC     MEM_OK

                MOV     SI,MEM_ERROR
                CALL    PRINT
                INT     0X20
MEM_OK:

; INITIERA VIDEOLéGE OCH SéTT ES TILL 0XB800

                MOV     AX,VMODE
                INT     0X10
                MOV     AX,0XB800
                MOV     ES,AX

                MOV     BYTE[BUFF],0

; FILL2 TôMMER SKRIVAREAN

                CALL    FILL2
                CALL    FILLLAST
                MOV     DX,MAXY*0X100+1
                MOV     AH,2
                XOR     BX,BX
                INT     0X10
                MOV     SI,MSG_LOADING
                CALL    PRINT

; BEHANDLA KOMMANDORADEN Sè ATT 0XD ERSéTTS MED 0, OCH BX PEKAR Pè FILNAMNET

                CMP     BYTE[0X80],0
                JZ      AGAIN
                MOV     SI,0X81
                XOR     BX,BX
PARAM0:         LODSB
                CMP     BX,0
                JNZ     PARAM2
                CMP     AL,' '
                JZ      PARAM1
                MOV     BX,SI
                DEC     BX
                JMP     SHORT PARAM1
PARAM2:         CMP     AL,' '
                JZ      PARAM3
PARAM1:         CMP     AL,0XD
                JNZ     PARAM0
PARAM3:         DEC     SI
                MOV     BYTE[SI],0

; FôRSôK ôPPNA FILEN

                MOV     AX,0X3D00
                MOV     DX,BX
                INT     0X21
                JC      AGAIN

; KOPIERA KOMMANDORADSARGUMENTER TILL 'FNAME'

                PUSH    AX
                MOV     BX,AX
                MOV     SI,FNAME
                CALL    STRCPY
                POP     AX

; LADDA FILEN

                JMP     LOAD_FILE
AGAIN:

; RéKNA UT ANTALET RADER OCH UPPDATERA BILDEN

                CALL    CALC_ENTER
                CALL    FILLLAST
                MOV     SI,BUFF
                MOV     CX,WORD[BEGLINE]
                CALL    GET_TO
                MOV     WORD[BUFF_PTR],SI
                CALL    UPDATE_TEXT

; PLACERA MARKôREN Pè RéTT STéLLE, OCH VéNTA Pè TANGENTTRYCKNING

                MOV     AH,2
                XOR     BX,BX
                MOV     DL,BYTE[CURSORX]
                MOV     DH,BYTE[CURSORY]
                INT     0X10
                XOR     AH,AH
                INT     0X16

                CMP     AH,0X52
                JNZ     NOT_INSERT
                XOR     BYTE[INSERT_FLAG],1
                JMP     KEY_DONE
NOT_INSERT:

; OM MAN TRYCKT Pè VéNSTERPIL...

                CMP     AH,0X4B
                JNZ     NOT_LEFT
                CMP     BYTE[CURSORX],0
                JZ      LEFT1
                DEC     BYTE[CURSORX]
LEFT1:          JMP     DONE_KEY
NOT_LEFT:

; OM MAN TRYCKT Pè HôGERPIL...

                CMP     AH,0X4D
                JNZ     NOT_RIGHT
                CALL    GET_ONE
                LODSB
                CMP     AL,0XD
                JZ      RIGHT1
                CMP     AL,0X0
                JZ      RIGHT1
                INC     BYTE[CURSORX]
RIGHT1:         JMP     DONE_KEY
NOT_RIGHT:

; OM MAN TRYCKT Pè UPPèTPIL...

                CMP     AH,0X48
                JNZ     NOT_UP
                CALL    DO_UP
                JMP     DONE_KEY
NOT_UP:

; OM MAN TRYCKT Pè NEDèTPIL...

                CMP     AH,0X50
                JNZ     NOT_DOWN
                MOV     CX,WORD[BEGLINE]
                ADD     CX,WORD[CURSORY]
                CMP     CX,WORD[MAXLINE]
                JNB     DOWN1
                CMP     BYTE[CURSORY],MAXY-1
                JNB     DOWN2
                INC     BYTE[CURSORY]
                MOV     BYTE[CURSORX],0
                JMP     KEY_DONE
DOWN2:          INC     WORD[BEGLINE]
                CALL    FILL
                MOV     BYTE[CURSORX],0
DOWN1:          JMP     KEY_DONE
NOT_DOWN:

; OM MAN TRYCKT Pè PAGE UP...

                CMP     AH,0X49
                JNZ     NOT_PGUP
                CMP     WORD[BEGLINE],0
                JNZ     PGUP2
                MOV     BYTE[CURSORY],0
                JMP     SHORT PGUP1
PGUP2:          CALL    FILL
                CMP     WORD[BEGLINE],MAXY
                JNB     PGUP0
                MOV     WORD[BEGLINE],0
                MOV     BYTE[CURSORY],0
                JMP     SHORT PGUP1
PGUP0:          SUB     WORD[BEGLINE],MAXY
PGUP1:          JMP     KEY_DONE
NOT_PGUP:

; OM MAN TRYCKT Pè PAGE DOWN...

                CMP     AH,0X51
                JNZ     NOT_PGDOWN
                CALL    FILL
                MOV     AX,WORD[MAXLINE]
                CMP     AX,MAXY
                JNA     PGDOWN0
                SUB     AX,MAXY
                CMP     WORD[BEGLINE],AX
                JNB     PGDOWN0
                ADD     WORD[BEGLINE],MAXY
                JMP     SHORT PGDOWN1
PGDOWN0:        MOV     AX,WORD[MAXLINE]
                MOV     WORD[BEGLINE],AX
                MOV     BYTE[CURSORY],0
                MOV     BYTE[CURSORX],0
PGDOWN1:        JMP     KEY_DONE
NOT_PGDOWN:

; OM MAN TRYCKT Pè TAB...

                CMP     AH,0X0F
                JNZ     NOT_TAB
                MOV     AX,WORD[CURSORX]
                MOV     CL,BYTE[TAB_SIZE]
                DIV     CL
                SUB     CL,AH
                XOR     CH,CH
                PUSH    CX
                CALL    GET_ONE
                POP     CX
                ADD     BYTE[CURSORX],CL
TAB0:           PUSH    SI
                PUSH    CX
                CALL    INSERT
                POP     CX
                POP     SI
                MOV     BYTE[SI],' '
                LOOP    TAB0
                JMP     KEY_DONE
NOT_TAB:

; OM MAN TRYCKT Pè DELETE...

                CMP     AH,0X53
                JNZ     NOT_DEL
                CALL    FILL
                CALL    GET_ONE
                CMP     BYTE[SI],0
                JZ      DEL1
DEL0:           INC     SI
                CALL    DELETE
DEL1:           JMP     KEY_DONE
NOT_DEL:

; OM MAN TRYCKT Pè KEY_MARK1...

                CMP     AH,KEY_MARK1 ;0X3C
                JNZ     NOT_F2
                CALL    GET_ONE
                MOV     WORD[MARK1],SI
                MOV     WORD[MARK2],SI
                JMP     KEY_DONE
NOT_F2:

; OM MAN TRYCKT Pè KEY_MARK2...

                CMP     AH,KEY_MARK2
                JNZ     NOT_F3
                CALL    GET_ONE
                MOV     WORD[MARK2],SI
                CMP     WORD[MARK2],SI
                JNB     F30
                MOV     AX,WORD[MARK2]
                MOV     WORD[MARK2],SI
                MOV     WORD[MARK1],AX
F30:            JMP     KEY_DONE
NOT_F3:

; OM MAN TRYCKT Pè KEY_DELBLOCK...

                CMP     AH,KEY_DELBLOCK
                JNZ     NOT_F4
                MOV     CX,WORD[MARK2]
                SUB     CX,WORD[MARK1]
                JNA     F41
                MOV     SI,WORD[MARK1]
                INC     SI
F40:            PUSH    CX
                PUSH    SI
                CMP     BYTE[SI],0
                JZ      F41
                CMP     BYTE[SI],0X0D
                JNZ     F44
                ;DEC     WORD[MAXLINE]
                CMP     BYTE[CURSORY],0
                JNZ     F43
                CMP     WORD[BEGLINE],0
                JZ      F42
                DEC     WORD[BEGLINE]
                JMP     SHORT F42
F43:            DEC     BYTE[CURSORY]
                JMP     SHORT F42
F44:            CMP     BYTE[CURSORX],0
                JZ      F42
                DEC     BYTE[CURSORX]
F42:            CALL    DELETE
                POP     SI
                POP     CX
                LOOP    F40
                MOV     AX,WORD[MARK1]
                MOV     WORD[MARK2],AX
F41:            CALL    FILL
                JMP     KEY_DONE
NOT_F4:

; OM MAN TRYCKT Pè KEY_COPY_INSERT...

                CMP     AH,KEY_COPY_INSERT
                JNZ     NOT_F5
                MOV     CX,WORD[MARK2]
                SUB     CX,WORD[MARK1]
                JNA     F50
                XOR     DX,DX
F51:            PUSH    CX
                PUSH    DX
                CALL    GET_ONE
                MOV     BX,SI
                ;CMP     SI,WORD[MARK1]
                ;JNA     F55
                ADD     BX,DX
F55:            
                ADD     SI,DX
                CALL    INSERT
                MOV     SI,WORD[MARK1]
                ADD     SI,DX
                CMP     SI,BX
                JB      F56
                ADD     SI,DX
                INC     SI
F56:
                ;MOV     AL,WORD[SI]
                LODSB
                MOV     BYTE[DS:BX],AL
                ;CALL    DELETE
                POP     DX
                INC     DX
                POP     CX
                LOOP    F51
                CALL    FILL
                MOV     AX,WORD[MARK1]
                MOV     WORD[MARK2],AX
F50:            JMP     KEY_DONE
NOT_F5:

; OM MAN TRYCKT Pè BACKSPACE

                CMP     AL,0X8
                JNZ     NOT_BACK
                CALL    FILL
                CMP     BYTE[CURSORX],0
                JNZ     BACK2
                CMP     WORD[BEGLINE],0
                JNZ     BACK3
                CMP     BYTE[CURSORY],0
                JZ      BACK1
BACK3:
                ;DEC     WORD[MAXLINE]
                CALL    GET_ONE
                CALL    DELETE
                CALL    DO_UP
                ;JMP     DO_END
                JMP     KEY_DONE
BACK2:          CALL    GET_ONE
                CALL    DELETE
                DEC     BYTE[CURSORX]
BACK1:          JMP     KEY_DONE
NOT_BACK:       

; OM MAN TRYCK Pè HOME...

                CMP     AH,0X47
                JNZ     NOT_HOME
                CALL    GET_ONE
HOME1:          DEC     SI
                MOV     AL,BYTE[SI]
                DEC     BYTE[CURSORX]
                CMP     AL,0XD
                JZ      HOME2
                CMP     SI,BUFF
                JZ      HOME3
                JMP     HOME1
HOME2:          INC     BYTE[CURSORX]
HOME3:          JMP     DONE_KEY
NOT_HOME:

; OM MAN TRYCK Pè END...

                CMP     AH,0X4F
                JNZ     NOT_END
DO_END:
                CALL    GET_ONE
END1:           INC     BYTE[CURSORX]
                LODSB
                CMP     AL,0XD
                JZ      END2
                CMP     AL,0
                JZ      END2
                JMP     END1
END2:           DEC     BYTE[CURSORX]
                JMP     DONE_KEY
NOT_END:

; OM MAN TRYCK Pè KEY_SAVE...

                CMP     AH,KEY_SAVE
                JNZ     NOT_SAVE
                JMP     DO_SAVE
NOT_SAVE:

; OM MAN TRYCK Pè ESCAPE...

                CMP     AH,01
                JZ      DO_ESCAPE
                JMP     NOT_ESCAPE
DO_ESCAPE:      CALL    FILLLAST
                MOV     DH,MAXY
                XOR     DL,DL
                MOV     AH,2
                INT     0X10
                MOV     SI,MENU
                CALL    PRINT
                XOR     AH,AH
                INT     0X16
                CMP     AH,0X2D
                JNZ     NOT_ESCAPE_EXIT
                MOV     AX,3                    ;   !!! AVSLUTA MICED !!!
                INT     0X10                    ; *************************
                INT     0X20
NOT_ESCAPE_EXIT:
                CMP     AH,0X1E
                JZ      SAVEAS                  ; SPARA SOM?
                CMP     AH,0X1F
                JNZ     NOT_ESCAPE_SAVE         ; BARA SPARA?
DO_SAVE:        CMP     BYTE[FNAME],0           ; JEPP!
                JNZ     SAVE2
SAVEAS:         CALL    FILLLAST                ; SPARA SOM BôRJAR HéR.
                MOV     DH,MAXY
                MOV     DL,0
                MOV     AH,2
                INT     0X10
                MOV     SI,FNAME
                CALL    INPUT
SAVE2:
                CALL    SET_A                   ; ENTER-SEKVENS 0XA,0XD
                XOR     CX,CX
                MOV     AH,0X3C
                MOV     DX,FNAME
                INT     0X21
                JC      SAVE3                   ; ERROR! SPARA INTE
                XCHG    AX,BX
                MOV     AH,0X40
                MOV     DX,BUFF
                XOR     CX,CX
                MOV     SI,BUFF
SAVE0:          INC     CX
                LODSB
                CMP     AL,0
                JNZ     SAVE0
                DEC     CX
                INT     0X21
                MOV     AH,0X3E
                INT     0X21
SAVE3:          CALL    REMOVE_A                ; ENTER-SEKVENS 0XD
                JMP     KEY_DONE
NOT_ESCAPE_SAVE:
                CMP     AH,0X18
                JNZ     NOT_ESCAPE_OPEN
                CALL    FILLLAST
                MOV     DH,MAXY
                MOV     DL,0
                MOV     AH,2
                INT     0X10
                MOV     SI,FNAME
                CALL    INPUT
                XOR     CX,CX
                MOV     AX,0X3D00
                MOV     DX,FNAME
                INT     0X21
LOAD_FILE:      XCHG    AX,BX
                MOV     AX,0X4202
                XOR     DX,DX
                MOV     CX,DX
                INT     0X21
                PUSH    AX
                MOV     AX,0X4200
                XOR     DX,DX
                MOV     CX,DX
                INT     0X21
                MOV     AH,0X3F
                MOV     DX,BUFF
                POP     CX
                PUSH    CX
                INT     0X21
                MOV     AH,0X3E
                INT     0X21
                POP     SI
                ADD     SI,BUFF
                MOV     BYTE[SI],0
                CALL    FILL
                CALL    REMOVE_A
                JMP     KEY_DONE
NOT_ESCAPE_OPEN:
                CMP     AH,0X31
                JNZ     NOT_ESCAPE_NEW
                MOV     BYTE[BUFF],0
                MOV     BYTE[CURSORX],0
                MOV     BYTE[CURSORY],0
                CALL    FILL
NOT_ESCAPE_NEW:
                CALL    FILLLAST
                JMP     KEY_DONE
NOT_ESCAPE:
                CMP     AL,0
                JNZ     ASCII
                JMP     KEY_DONE
ASCII:          CMP     BYTE[CURSORX],MAXX
                JNB     ASCII_DONE
                PUSH    AX
                CALL    GET_ONE
                CMP     BYTE[SI],0XD
                JZ      ASCII3
                CMP     BYTE[SI],0
                JZ      ASCII3
                CMP     BYTE[INSERT_FLAG],1
                JZ      ASCII1
ASCII3:         PUSH    SI
                CALL    INSERT
                POP     SI
                JMP     SHORT ASCII1
ASCII1:         POP     AX
                MOV     BYTE[SI],AL
                INC     BYTE[CURSORX]
ASCII_DONE:

                CMP     AL,0XD
                JNZ     NOT_ENTER
DO_ENTER:       INC     WORD[MAXLINE]
                MOV     BYTE[CURSORX],0
                CMP     BYTE[CURSORY],MAXY-1
                JB      ENTER1
                INC     WORD[BEGLINE]
                JMP     ENTER2
ENTER1:         INC     BYTE[CURSORY]
ENTER2:         CALL    FILL

NOT_ENTER:
KEY_DONE:
DONE_KEY:       JMP     AGAIN

GET_ONE:        MOV     SI,BUFF
                MOV     CX,WORD[BEGLINE]
                ADD     CX,WORD[CURSORY]
                CALL    GET_TO
                ADD     SI,WORD[CURSORX]
                RET

GET_TO:         INC     CX
GET_TO2:        DEC     CX
                JZ      GET_TO_QUIT                
GET_TO3:        LODSB
                CMP     AL,0XD
                JZ      GET_TO2
                JMP     GET_TO3
GET_TO_QUIT:    RET

FILL:
FILL2:          XOR     DI,DI
                MOV     AH,BYTE[COLOR]
                MOV     CX,MAXX*MAXY
FILL3:          MOV     AL,' '
                REP     STOSW
                RET

FILLLAST:       MOV     DI,MAXX*MAXY*2
                MOV     CX,MAXX
                MOV     AH,BYTE[LASTCOLOR]
                CALL    FILL3
                ;MOV     AL,' '
                ;REP     STOSW
                CMP     BYTE[INSERT_FLAG],0
                JZ      FILL4
                MOV     BYTE[ES:MAXX*TOTY*2-4],'*'
FILL4:          RET

UPDATE_TEXT:    MOV     SI,WORD[BUFF_PTR]
                XOR     DI,DI
UPDATE_LOOP:    MOV     AL,BYTE[CURY]
                MOV     AH,MAXX*2
                MUL     AH
                ADD     AX,WORD[CURX]
                MOV     DI,AX
UPDATE_LOOP2:   LODSB
                CMP     AL,0
                JZ      UPDATE_QUIT1
                CMP     AL,0XD
                JZ      UPDATE_NEWLINE
                STOSB
                CMP     SI,WORD[MARK1]
                JNA     UPDATE0
                CMP     SI,WORD[MARK2]
                JA      UPDATE0
                MOV     AL,BYTE[MARKCOLOR]
                JMP     UPDATE1
UPDATE0:
                MOV     AL,BYTE[COLOR]
UPDATE1:
                STOSB
                INC     WORD[CURX]
                CMP     WORD[CURX],MAXX
                JB      UPDATE_LOOP2
UPDATE2:        LODSB
                CMP     AL,0
                JZ      UPDATE_QUIT
                CMP     AL,0XD
                JZ      UPDATE_LOOP2
                JMP     UPDATE2
UPDATE_NEWLINE: MOV     WORD[CURX],0
                INC     BYTE[CURY]
                CMP     BYTE[CURY],MAXY
                JNB     UPDATE_QUIT
                JMP     SHORT UPDATE_LOOP
UPDATE_QUIT1:   MOV     AL,END_SIGN
                STOSB
                MOV     AL,BYTE[COLOR]
                STOSB
UPDATE_QUIT:    MOV     WORD[CURX],0
                MOV     BYTE[CURY],0
                RET

DELETE:         LODSB
                MOV     BYTE[SI-2],AL
                CMP     AL,0
                JNZ     DELETE
                RET

INSERT:         XOR     CX,CX
INSERT0:        LODSB
                INC     CX
                CMP     AL,0
                JNZ     INSERT0
                INC     CX
INSERT1:        DEC     SI
                MOV     AL,BYTE[SI]
                MOV     BYTE[SI+1],AL
                LOOP    INSERT1
                RET

INPUT:          PUSH    ax
                push    cx
                mov     cx,1
input2:         mov     ah,0
                int     0X16
                cmp     al,0X0d
                jz      input0
                cmp     al,0X08
                jz      input1
                inc     cx
                mov     BYTE[si],al
                inc     si
                mov     ah,0X0e
                int     0X10
                jmp     input2
input0:         mov     al,0
                mov     BYTE[si],al
                inc     si
                pop     cx
                pop     ax
                ret
input1:         cmp     cx,1
                jz      input2
                dec     cx
                dec     si
                mov     ah,0X0e
                int     0X10
                mov     al,' '
                int     0X10
                mov     al,0X08
                int     0X10
                jmp     input2

PRINT:          LODSB
                CMP     AL,0
                JZ      PRINT0
                MOV     AH,0XE
                INT     0X10
                JMP     PRINT
PRINT0:         RET

CALC_ENTER:     XOR     CX,CX
                MOV     SI,BUFF
CALC_ENTER0:
                LODSB
                CMP     AL,0
                JZ      CALC_ENTER_EXIT
                CMP     AL,0XD
                JNZ     CALC_ENTER0
                INC     CX
                JMP     CALC_ENTER0
CALC_ENTER_EXIT: MOV    WORD[MAXLINE],CX
                RET

DO_UP:          MOV     BYTE[CURSORX],0
                CMP     BYTE[CURSORY],0
                JZ      UPP1
                DEC     BYTE[CURSORY]
                RET
UPP1:           CMP     WORD[BEGLINE],0
                JZ      UPP2
                DEC     WORD[BEGLINE]
                CALL    FILL
UPP2:           RET

REMOVE_A:       MOV     SI,BUFF
REMOVE0:        LODSB
                CMP     AL,0
                JZ      REMOVE1
                CMP     AL,0XA
                JNZ     REMOVE0
                PUSH    SI
                CALL    DELETE
                POP     SI
                JMP     REMOVE0
REMOVE1:        RET

SET_A:          MOV     SI,BUFF
SET0:           LODSB
                CMP     AL,0
                JZ      SET1
                CMP     AL,0XD
                JNZ     SET0
                PUSH    SI
                INC     SI
                CALL    INSERT
                MOV     BYTE[SI],0XA
                POP     SI
                JMP     SET0
SET1:           RET

STRCPY:         MOV     AL,BYTE[DS:BX]
                CMP     AL,0
                JZ      STRCPY0
                MOV     BYTE[SI],AL
                INC     SI
                INC     BX
                JMP     STRCPY
STRCPY0:        RET

COLOR:          DB      0X1E
LASTCOLOR:      DB      0X70
MARKCOLOR:      DB      0X71
CURX:           DW      0
CURY:           DB      0
CURSORX:        DB      0
                DB      0
CURSORY:        DB      0
                DB      0
BEGLINE:        DW      0
MAXLINE:        DW      0
BUFF_PTR:       DW      0
MARK1:          DW      0
MARK2:          DW      0
TAB_SIZE:       DB      8
INSERT_FLAG:    DB      0
MEM_ERROR:      DB      "OUT OF MEM",0
MSG_LOADING:    DB      "LOADING...",0
MENU:           DB      " save [A]s e[X]it [S]ave [O]pen [N]ew",0
FNAME           EQU     $
BUFF            EQU     FNAME+80
TOTY            EQU     25
MAXY            EQU     24
MAXX            EQU     80
VMODE           EQU     3
KEY_SAVE        EQU     0X3C
KEY_COPY_INSERT EQU     0X40
KEY_DELBLOCK    EQU     0X3F
KEY_MARK1       EQU     0X3D
KEY_MARK2       EQU     0X3E
END_SIGN        EQU     176
