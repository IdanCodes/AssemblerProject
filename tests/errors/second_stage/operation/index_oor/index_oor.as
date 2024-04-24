; index out of range (self explanatory)
str: .string "abc"
mov str[4], str[2]
dat: .data 1, 2, 3
mov dat[0], dat[3]
