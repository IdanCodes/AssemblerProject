;.define hi = 1024
;ENDSTR: .string "FINISHED!"
;HELLO: mov #hi, r1

;HI: .extern meow

;LOOPSTART: prn #hi
;dec r1
;cmp r1, #0
;bne LOOPSTART

;prn ENDSTR

;.define index = 1ג
;ARR: .data 2, 3, index
;mov r5, r2

;.define sz = 2
;MAIN:   mov r3, LIST[sz]
;LOOP:   jmp L1
;        prn #-5
;        mov STR[5], STR[2]
;        sub r1, r4
;        cmp r3, #sz
;        bne END     
;L1:     inc K
;        bne LOOP
;END:    hlt
;.define len = 4
;STR:    .string "abcdef"
;LIST:   .data 611, -9, len
;K:      .data   22

hlt
HI: hlt
hlt
.entry HI