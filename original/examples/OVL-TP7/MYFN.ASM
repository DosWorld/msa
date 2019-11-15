testfn:
      MOV  AX,44
      RETF
putc:
      push bp
      mov  bp,sp
      mov  dl,byte[ss:bp+6]
      mov  ah,2
      int  0x21
      mov  sp,bp
      pop  bp
      retf 2

      export testfn
      export putc
