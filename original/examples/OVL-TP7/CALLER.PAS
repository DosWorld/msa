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