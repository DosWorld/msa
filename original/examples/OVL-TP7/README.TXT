
  Here is example how to use overlay with TP6/TP7.

  ===

  1. Borland has own engine for overlays, but for some cases
it could be usefull - for example, for some reasons BGI
does not use standart overlays.

  2. Turbo Pascal is universal programming language - it
is easy way how to explain usage model.

So..

  In this example CALLER.PAS load MYFN.OVL and call two
functions: TESTFN and PUTC.

  ===

  MSA2 overlays does not describe how to parameters
must be passed to overlay, so you can use with
your programming language convention.

In this example - used pascal convention with far calls.

  ===

  To build this example - type 'make', or

compile overlay with:

     MSA2 MYFN.ASM -oMYFN.OVL -fovl

compile pascal sources:

     TPC /M CALLER.PAS
