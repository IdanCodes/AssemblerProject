; a program to write a width x height grid filled with a character - a "matrix"
.extern width
.extern height

; export the matrix character & newLine
.entry matChar
.entry newLine

.define start = 1

; character for each element of the matrix
matChar: .string "0"
spaceChar: .string " "
; newline character - carriage return + line feed
newLine: .data 10, 13

i: .data start
j: .data start

; printing the matrix
printRow: mov #start, i
printChar: prn matChar[0]
prn spaceChar[0]

inc i[0]
cmp i[0], width[0]
bne printRow

; i == width => print newline
prn newLine[0]
prn newLine[1]

inc j[0]
cmp j[0], height[0]
bne printRow
