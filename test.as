; .define hi = 2
; ENDSTR: .string "FINISHED!"
; HELLO: mov hi, r1

; LOOPSTART: prn #hi
; dec r1
; cmp r1, 0
; bne LOOP_START

; prn END_STR

; .define meow = 4
; MYARR: .data 2, meow, 4
; .data 21, -12

.define a13a213a = 2
meow: .data 1,2 ,3,2   2, 222, 3, 3,3 
MEOW: .extern hola
