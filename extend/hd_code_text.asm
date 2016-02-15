// Blast Corps (U) (V1.1) patch to hd_code_text.raw to use ROM lookup table
// Updated: 2016 Feb 15
// By: queueRAM
// For updates, check: http://origami64.net/showthread.php?tid=334&pid=2566

// Use with N64 version of bass: https://github.com/ARM9/bass
// > bass -o hd_code_text.raw hd_code_text.asm

arch n64.cpu
endian msb

include "mips4300.inc"

// hd_code_text.raw offset 0x119BC is RAM 0x8025617C
origin 0x119BC

sll   t6, t6, 0x4    // each entry is four words
lui   at, 0xB080     // %hi(0xB07FC000), 0xB0000000 = ROM
addu  at, at, t6     //
lw    t6, 0xC000(at) // %lo(0xB07FC000)
sw    t6, 0x24(sp)   //
lw    t7, 0xC00C(at) // %lo(0xB07FC00C)
sub   t7, t7, t6     // compute length
lw    t4, 0x30(sp)   //
sw    t7, 0x0(t4)    //
b     0x12240        // skip over old code

// fill existing code with nops to allow the new gzip code to be smaller than the original
fill 0x12240-pc()