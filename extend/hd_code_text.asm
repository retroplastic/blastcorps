arch n64.cpu
endian msb

include "N64.INC"

origin 0x119BC

sll   t6, t6, 0x3    // each entry is two words
lui   at, 0xB080     // %hi(0xB07FA000), 0xB0000000 = ROM
addu  at, at, t6     //
lw    t6, 0xA000(at) // %lo(0xB07FA000)
sw    t6, 0x24(sp)   //
lw    t7, 0xA004(at) // %lo(0xB07FA004)
sub   t7, t7, t6     //
lw    t4, 0x30(sp)   //
sw    t7, 0x0(t4)    //
b     0x12240        //

// fill with nops to allow the new gzip code to be smaller than the original
fill 0x12240-pc()