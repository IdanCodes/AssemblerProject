;.define hi = 1024
;ENDSTR: .string "FINISHED!"
;HELLO: mov #hi, r1

;HI: .extern meow

;LOOPSTART: prn #hi
;dec r1
;cmp r1, #0
;bne LOOPSTART

;prn ENDSTR

.define index = 1
ARR: .data 2, 3, index
prn ARR[index]
prn #index
prn #-5
