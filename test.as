.define hi = 100
ENDSTR: .string "FINISHED!"
HELLO: mov #hi, r1

HI: .extern meow 

LOOPSTART: prn #hi
dec r1
cmp r1, #0
bne LOOPSTART

prn ENDSTR
