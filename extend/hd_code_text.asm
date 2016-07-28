// Blast Corps (U) (V1.1) patch to hd_code_text.raw to use ROM lookup table
// Updated: 2016 Feb 15
// By: queueRAM
// For updates, check: http://origami64.net/showthread.php?tid=334&pid=2566

// Use with N64 version of bass: https://github.com/ARM9/bass
// > bass -o hd_code_text.raw hd_code_text.asm

arch n64.cpu
endian msb

include "mips4300.inc"

// void osInvalDCache(void *vaddr, s32 nbytes);
constant osInvalDCache(0x802D6740)
// s32 osPiStartDma(OSIoMesg *mb, s32 priority, s32 direction, 
//                  u32 devAddr, void *vAddr, u32 nbytes, OSMesgQueue *mq);
constant osPiStartDma(0x802DA2F0)
// s32 osRecvMesg(OSMesgQueue *mq, OSMesg *msg, s32 flag);
constant osRecvMesg(0x802D4910)

constant InitiateDma(0x8028B4C4) // 046D04

// I/O message priority
constant OS_MESG_PRI_NORMAL(0)
constant OS_MESG_PRI_HIGH(1)

// Flags to indicate direction of data transfer
constant OS_READ(0) // device -> RDRAM
constant OS_WRITE(1) // device <- RDRAM

// Flags to turn blocking on/off when sending/receiving message
constant OS_MESG_NOBLOCK(0)
constant OS_MESG_BLOCK(1)

// hd_code_text.raw offset 0x1199C is RAM 0x8025615C
origin 0x1199C
base 0x8025615C
LoadLevel:
  // begin no changes
  addiu sp, sp, -0x28
  sw    a0, 0x28(sp)
  lw    t6, 0x28(sp)
  sw    ra, 0x1c(sp)
  sw    a1, 0x2c(sp)
  sltiu at, t6, 0x3c
  beqz  at, LoadLevel_8A4
  sw    a2, 0x30(sp)
  // end no changes

  // 80308B98 contains the old jump table - use it to DMA table entries
  // align it to 80308BA0

  // osInvalDCache(vAddr = 0x80308BA0, numBytes = 0x10)
  addiu a1, zero, 0x10
  lui   a0, 0x8030
  jal   osInvalDCache
  ori   a0, 0x8BA0

  // osPiStartDma(0x80370C58, OS_MESG_PRI_NORMAL, OS_READ, devAddr, 0x80308BA0, 0x10, 0x803150A0);
  lw    t0, 0x28(sp) // load level id (a0)
  sll   t0, t0, 0x4 // * 0x10
  lui   a3, 0x7F
  ori   a3, a3, 0xC000
  addu  a3, t0, a3 // devAddr = 0x7FC000 + levelId * 0x10
  lui   t0, 0x8030
  ori   t0, t0, 0x8BA0
  sw    t0, 0x10(sp)
  addiu t0, zero, 0x10
  sw    t0, 0x14(sp)
  lui   t0, 0x8031
  ori   t0, 0x50A0
  sw    t0, 0x18(sp)
  lui   a0, 0x8037
  addiu a0, a0, 0x0C58
  addiu a2, zero, OS_READ
  jal   osPiStartDma
  addiu a1, zero, OS_MESG_PRI_NORMAL

  // osRecvMesg(0x803150A0, NULL, OS_MESG_BLOCK);
  lui   a0, 0x8031
  ori   a0, a0, 0x50A0
  addiu a1, zero, 0x0
  jal   osRecvMesg
  addiu a2, zero, OS_MESG_BLOCK


  lui   at, 0x8031     // %hi(0x80308BA0)
  lw    t6, 0x8BA0(at) // %lo(0x80308BA0)
  sw    t6, 0x24(sp)   //
  lw    t7, 0x8BAC(at) // %lo(0xB07FC00C)
  sub   t7, t7, t6     // compute length
  lw    t4, 0x30(sp)   //
  sw    t7, 0x0(t4)    // store length = *a2
  b     LoadLevel_8A4  // skip over old code
  nop

// fill existing code with nops to allow the new gzip code to be smaller than the original
fill 0x80256A00-pc()

LoadLevel_8A4: // 80256A00
  // begin no changes
  addiu t7, zero, 0xa
  addiu t8, zero, 1
  sw    t8, 0x14(sp)
  sw    t7, 0x10(sp)
  lw    a0, 0x24(sp)
  lw    a1, 0x2c(sp)
  lw    a2, 0x30(sp)
  jal   InitiateDma
  addiu a3, zero, 0xc
  lw    ra, 0x1c(sp)
  addiu sp, sp, 0x28
  jr    ra
  nop
  // end no changes
// end LoadLevel
