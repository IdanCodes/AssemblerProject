; a constant's name is the same as a macro's name
mcr x
mov r1, r2
endmcr

.define x = 2

; the same error will be shown if the constant was defined before the macro
