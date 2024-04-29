weAreCloseTo: .string "The Edge"
theAlphabet: .string "abc"
naturals: .data 1, 2, 3, 4
.string "NIGGA"
.string " NIGGA 1   "

.string "2 NIGGA "



; here we define the Dat dat
.define a22a=2

DATDAT: .data 1 ,2, 1, a22a, 3, 0

mov DATDAT[0 ]  ,weAreCloseTo[     2]       
    prn         DATDAT
mov         theAlphabet[0], theAlphabet[   a22a ]
mov theAlphabet[ a22a ] ,           theAlphabet[2]

mov naturals[ 2],naturals[3]

closeToStr: .string "We Are Close To "
prn closeToStr


prn weAreCloseTo

prn newLine

newLine: .data 10, 13, 0
