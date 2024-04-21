; a label is defined in the beginning of an extern instruction
LABEL: .extern extLbl

; the label is completely ignored in the .extern line, so it can be "re"-declared
LABEL: .string "Hi!"