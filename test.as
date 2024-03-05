.define hi = 2
ENDSTR: .string "FINISHED!"

HELLO: mov hi, r1

LOOPSTART: prn #hi
dec r1
cmp r1, 0
bne LOOP_START

prn END_STR
