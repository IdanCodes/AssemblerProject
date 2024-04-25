.entry i
.extern output

.define timesToPrint = 10

prnLoop: prn output
inc i
cmp i, #timesToPrint
bne prnLoop

i: .data 1
