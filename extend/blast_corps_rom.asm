arch n64.cpu
endian msb

include "N64.INC"

// TODO: update end offset of code
// origin 0x2A4C
// addiu a3, a3, 0x3acc

// new code
origin 0x787FD0
insert "bin/hd_code_text.raw.gz"
if pc() > 0x7D73B4 {
   error "Code > 0x7D73B4"
}

fill 0x7D73B4-pc()

// LUT
origin 0x7FA000
dd 0x4ACC10, 0x4B8960
dd 0x4A5660, 0x4ACC10
dd 0x4B8960, 0x4BFD60
dd 0x4BFD60, 0x4C3AC0
dd 0x4C3AC0, 0x4D5F90
dd 0x4D5F90, 0x4E2F70
dd 0x4E2F70, 0x4E4E80
dd 0x4E4E80, 0x4E7C00
dd 0x4E7C00, 0x4E8F70
dd 0x4E8F70, 0x4F5C10
dd 0x4F5C10, 0x500520
dd 0x500520, 0x507E80
dd 0x507E80, 0x511340
dd 0x511340, 0x523080
dd 0x523080, 0x52CD00
dd 0x52CD00, 0x532700
dd 0x532700, 0x53E9B0
dd 0x53E9B0, 0x54A820
dd 0x54A820, 0x552DE0
dd 0x552DE0, 0x555000
dd 0x555000, 0x560E90
dd 0x560E90, 0x5652D0
dd 0x5652D0, 0x56F3F0
dd 0x56F3F0, 0x5721E0
dd 0x5721E0, 0x5736E0
dd 0x5736E0, 0x57A2C0
dd 0x57A2C0, 0x580B60
dd 0x580B60, 0x588CE0
dd 0x588CE0, 0x58BE80
dd 0x58BE80, 0x597B80
dd 0x597B80, 0x59B7D0
dd 0x59B7D0, 0x5A5840
dd 0x5A5840, 0x5B0B10
dd 0x5B0B10, 0x5B5A30
dd 0x5B5A30, 0x5B8BB0
dd 0x5B8BB0, 0x5C4C80
dd 0x5C4C80, 0x5CA9C0
dd 0x5CA9C0, 0x5CCF50
dd 0x5CCF50, 0x5D1060
dd 0x5D1060, 0x5DC830
dd 0x5DC830, 0x5E6EE0
dd 0x5E6EE0, 0x5EC800
dd 0x5EC800, 0x5F3A80
dd 0x5F3A80, 0x6014B0
dd 0x6014B0, 0x60A710
dd 0x60A710, 0x613AA0
dd 0x613AA0, 0x61DD70
dd 0x61DD70, 0x621AF0
dd 0x621AF0, 0x6269E0
dd 0x6269E0, 0x630C30
dd 0x630C30, 0x635700
dd 0x635700, 0x63CA10
dd 0x63CA10, 0x641F30
dd 0x641F30, 0x644810
dd 0x644810, 0x646080
dd 0x646080, 0x647550
dd 0x647550, 0x654FC0
dd 0x654FC0, 0x660950
dd 0x660950, 0x665F80
dd 0x665F80, 0x66C900
