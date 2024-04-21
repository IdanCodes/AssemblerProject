; a label is defined in the beginning of an entry instruction
switch: mov r0, r1
LABEL: .entry switch

; the label is completely ignored in the .entry line
; -it can be "re"-declared
LABEL: .data 1, 2