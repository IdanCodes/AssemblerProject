; defining a label with the same name as a macro
mcr originalName
mov r1, r2
endmcr

originalName: .string "Hi!"
; the same error will be shown if the label was defined before the macro
