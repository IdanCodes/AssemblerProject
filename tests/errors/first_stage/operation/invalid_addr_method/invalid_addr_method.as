; invalid addressing method for an operand
dat: .data 1, 2
; here r1, can't be a source operand
lea r1, dat[2]