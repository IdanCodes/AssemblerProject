; -- declarations
; define a constant variable
NUM: .define 10
; declare an array
    ARR: .data 4, 3, 1
; declare strings
    HI_STR: .string "HI"
    END_STR: .string "END"

; -- macro
mcr my_mcr
; add NUM to the value in r1
    add NUM, r1
; print the value in r1
    prn r1
endmcr

; -- print
prn HI_STR

; -- loop
; subtract 1 from NUM
LOOP:   sub NUM
; insert macro
        my_mcr
        cmp NUM, 0
; while NUM != 0 repeat the loop
        bne LOOP

prn END_STR
