; -- declarations
; define a constant valuekladsklasjdkljaskldasjkldlkasjdladkasjdlkaskldalsjdlajdlkajdlkasjdlkasjdlkasjdlkasjdlaskjdlaksjdalskdjlaksdjaslkdjaslkdjalskdjaslkdjaslkdjasdlkjasdlkjasdlkjasdlkasjdklj
    .define numMin = 0
; define a variable
    NUM: .data 10
; declare an array
    ARR: .data 4, 3, 1
; declare strings
    HISTR: .string "HI"
    ENDSTR: .string "END"

; -- macro
mcr my_mcr
; add NUM to the byte in r1
    add NUM, r1
; print the byte in r1
    prn r1
endmcr

; -- print
prn HISTR

; -- loop
; subtract 1 from num
LOOP:   dec NUM
; insert macro
        my_mcr
        cmp NUM, #numMin
; while NUM != 0 repeat the loop
        bne LOOP

prn ENDSTR
