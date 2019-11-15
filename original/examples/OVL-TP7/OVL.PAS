{

MIT License

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

}
unit ovl;
{$F+}

interface

type

TOvl = record
   aseg		:word;
   stab		:word;
   stab_hdr     :array[0..2] of word;
end;

function ovl_load(var ovlrec:TOvl; fname:string):boolean;
procedure ovl_unload(var ovlrec:TOvl);
function ovl_find(var ovlrec:TOvl; funcname:pchar;var p:pointer):boolean;

implementation

const
	MAGIK = $FE33;
	SYMBOL_LEN = 32;

function DosGetMem(count:word):word;
begin
  if (count and $f) <> 0 then count := (count shr 4) + 1
  else count := count shr 4;
  asm
	push	es
	push	ds
	push	bp
	mov	bx,count
	mov	ah,48h
	int	21h
	jnc	@c1
	xor	ax,ax
@c1:
	pop	bp
	pop	ds
	pop	es
	mov	count,ax
  end;
  DosGetMem := count;
end;

procedure DosFreeMem(aseg:word);assembler;
asm
	mov	ax,aseg
	or	ax,ax
	jz	@c2
	push	es
	push	ds
	push	bp
	mov	es,ax
	mov	ah,49h
	int	21h
	pop	bp
	pop	ds
	pop	es
@c2:
end;

function ovl_load(var ovlrec:TOvl; fname:string):boolean;
var exe_hdr:array[0..31] of byte;
    bss_size, lblock_size:word;
    code_size:longint;
    stab_hdr:array[0..2] of word;
    f:file;
    r:boolean;
    exe_ofs, stab_ofs:longint;
begin
	r := false;
        ovlrec.aseg := 0;
        ovlrec.stab := 0;
	{$I-}
	assign(f, fname);
	reset(f,1);
	{$I+}
	if ioresult = 0 then
	begin
		blockread(f, exe_hdr, 32);
		lblock_size := (exe_hdr[2] or (exe_hdr[3] shl 8));
		bss_size := ((exe_hdr[$0a] and $ff) or ((exe_hdr[$0b] and $ff) shl 8));
		code_size := (exe_hdr[4] or (exe_hdr[5] shl 8)) * 512;
		if lblock_size <> 0 then dec(code_size, 512 - lblock_size);

		exe_ofs := (exe_hdr[8] or (exe_hdr[9] shl 8)) * 16;
		stab_ofs := exe_ofs + code_size;
		ovlrec.aseg := DosGetMem(code_size + bss_size * 512);
		if ovlrec.aseg <> 0 then
		begin
	                {$I-}
	                seek(f, exe_ofs);
		        blockread(f, mem[ovlrec.aseg:0], code_size);
	                seek(f, stab_ofs);
	                blockread(f, ovlrec.stab_hdr, 6);
	                {$I+}
                        if (ovlrec.stab_hdr[0] = MAGIK) and (ioresult = 0) then
			begin
		        	ovlrec.stab := DosGetMem(ovlrec.stab_hdr[2]);
			        if ovlrec.stab<>0 then
				begin
			                {$I-}
				        blockread(f, mem[ovlrec.stab:0], ovlrec.stab_hdr[2]);
			                {$I+}
					r := true;
               			end;
			end;
		end;
		close(f);
	end;
	if not r then ovl_unload(ovlrec);
	ovl_load := r;
end;

procedure ovl_unload(var ovlrec:TOvl);
begin
     DosFreeMem(ovlrec.aseg);
     DosFreeMem(ovlrec.stab);
     ovlrec.aseg := 0;
     ovlrec.stab := 0;
end;

function cmp(p1, p2 :pchar; len:integer):boolean;
var r:boolean;
    i:integer;
begin
	r := false;
	i := 0;
	while i < len do
	begin
		if upcase(p1[i]) <> upcase(p2[i]) then break;
		if p2[i] = #0 then
		begin
			r := true;
			break;
		end;
		inc(i);
	end;
	cmp := r;
end;

function ovl_find(var ovlrec:TOvl; funcname:pchar;var p:pointer):boolean;
var
	r :boolean;
	i :integer;
	ofs:word;
begin
	r := false;
	p := nil;
	if (ovlrec.aseg<>0) and (ovlrec.stab<>0) and (ovlrec.stab_hdr[1]<>0) then
	begin
		for i:= 0 to ovlrec.stab_hdr[1] - 1 do
		begin
			if cmp(@mem[ovlrec.stab:i*36], funcname, SYMBOL_LEN) then
			begin
				ofs := (mem[ovlrec.stab:i*36+32] and $ff) or (mem[ovlrec.stab:i*36+33] and $ff) shl 8; 
				p := ptr(ovlrec.aseg, ofs);
				r := true;
				break;
			end;	
		end;
	end;
	ovl_find := r;
end;

end.
