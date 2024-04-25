; a program to print a sequence a_n defined like this:
; a_n = { n is odd -> 2n, n is even -> -n }

.entry n

; number elements to print
.define numPrint = 10

mcr printR1
add zero[0], r1
prn r1
endmcr

loop: mov n, r1
; "multiply by 2"
add r1, r1
printR1

inc n
; n is now even
mov #0, r1
sub n, r1
printR1

cmp n, #numPrint
bne loop

; the 'n' in a_n
n: .data 1

; the character '0' to print
zero: .string "0"
