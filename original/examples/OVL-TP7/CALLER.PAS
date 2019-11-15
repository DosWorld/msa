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
{$M 16000,0,0}
uses ovl;

{$F+}

type
	TMyProc = function:integer;
        TPutc   = procedure(a:char);

var
	o	:tovl;
	fn	:TMyProc;
	putc	:TPutc;

procedure puts(s:string);
var i:byte;
begin
  for i:=1 to ord(s[0]) do putc(s[i]);
end;

begin

   if not ovl_load(o, 'myfn.ovl') then
   begin
      writeln('Error while load overlay.');
      halt(1);
   end;

   writeln('Overlay loaded.');


   if ovl_find(o, 'TESTFN', @fn) then begin
      writeln('Overlay function TESTFN return ', fn, '.');
   end else begin
      writeln('Error - function TESTFN not found.');
   end;

   if ovl_find(o, 'PUTC', @putc) then puts('>> This print with overlay function PUTC.'+#13+#10);

   writeln('Unload overlay.');

   ovl_unload(o);

   writeln('Done.');

end.