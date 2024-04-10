.define LENGTH = 3
.define TOADD = 2
ARR: .data 1, 2, 3

; print the values of the array
mcr printArr
prn ARR[0]
prn ARR[1]
prn ARR[2]
endmcr

; add TOADD to each element in ARR
mcr addValues
add #TOADD, ARR[0]
add #TOADD, ARR[1]
add #TOADD, ARR[2]
endmcr

; reset the array's values to their start values
mcr resetValues
mov STARTVALUES[0], ARR[0]
mov STARTVALUES[1], ARR[1]
mov STARTVALUES[2], ARR[2]
endmcr

; save the start values of the array
STARTVALUES: .data 0, 0, 0
mov ARR[0], STARTVALUES[0]
mov ARR[1], STARTVALUES[1]
mov ARR[2], STARTVALUES[2]

; reset the values of the array
addValues
printArr
resetValues
printArr
addValues
printArr
