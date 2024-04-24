; constant index was out of range
mystring: .string "Hello, world!"
.define length = 13
prn #length
cmp mystring[0], mystring[14]