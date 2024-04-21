; there's a macro with the same name as the .extern argument
mcr originalName
mov r1, r2
endmcr

.extern originalName
; the same error will be shown if the .extern was written before the macro