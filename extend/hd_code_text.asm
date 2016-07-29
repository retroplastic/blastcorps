// Blast Corps (U) (V1.1) patch to hd_code_text.raw to use ROM lookup table
// Updated: 2016 Jul 28
// By: queueRAM
// For updates, check: http://origami64.net/showthread.php?tid=334&pid=2566

// Use with N64 version of bass: https://github.com/ARM9/bass
// > bass -o hd_code_text.raw hd_code_text.asm

arch n64.cpu
endian msb

include "mips4300.inc"

//-----------
// OS defines
//-----------

// I/O message priority
constant OS_MESG_PRI_NORMAL(0)
constant OS_MESG_PRI_HIGH(1)

// Flags to indicate direction of data transfer
constant OS_READ(0)  // device -> RDRAM
constant OS_WRITE(1) // device <- RDRAM

// Flags to turn blocking on/off when sending/receiving message
constant OS_MESG_NOBLOCK(0)
constant OS_MESG_BLOCK(1)

//-------------
// OS functions
//-------------

// void osInvalDCache(void *vaddr, s32 nbytes);
// s32 osPiStartDma(OSIoMesg *mb, s32 priority, s32 direction, 
//                  u32 devAddr, void *vAddr, u32 nbytes, OSMesgQueue *mq);
// s32 osRecvMesg(OSMesgQueue *mq, OSMesg *msg, s32 flag);

constant VER_J(0)
constant VER_U0(1)
constant VER_U1(2)
constant VER_E(3)

if !{defined VERSION} {
  error "Must define VERSION: U1.0, U1.1, J"
}

//-----------
// (U) (V1.0)
//-----------
if {VERSION} == VER_U0 {
  constant osInvalDCache(0x802D6690)
  constant osPiStartDma(0x802DA240)
  constant osRecvMesg(0x802D4860)

  constant InitiateDma(0x8028B4C4)

  constant JUMP_TABLE_BASE(0x80308AF0)
}

//-----------
// (U) (V1.1)
//-----------
if {VERSION} == VER_U1 {
  constant osInvalDCache(0x802D6740)
  constant osPiStartDma(0x802DA2F0)
  constant osRecvMesg(0x802D4910)

  constant InitiateDma(0x8028B4C4)

  constant JUMP_TABLE_BASE(0x80308BA0)
}

//-----
// (J)
//-----
if {VERSION} == VER_J {
  constant osInvalDCache(0x802D6B10)
  constant osPiStartDma(0x802DA6C0)
  constant osRecvMesg(0x802D4CE0)

  constant InitiateDma(0x8028B824)

  constant JUMP_TABLE_BASE(0x80308E30)
}

//---------
// New code
//---------

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
  beqz  at, LoadLevel_Dma
  sw    a2, 0x30(sp)
  // end no changes

  // Old jump tables - use these areas to DMA table entries
  // align to 0x10
  // (U) (V1.0): 0x80308AE8
  // (U) (V1.1): 0x80308B98
  // (J):        0x80308E28
  // (E):
  // 80308B98 contains the old jump table - use it to DMA table entries

  // osInvalDCache(vAddr = 0x80308BA0, numBytes = 0x10)
  lui   a0, JUMP_TABLE_BASE >> 16
  ori   a0, JUMP_TABLE_BASE & 0xFFFF
  jal   osInvalDCache
  addiu a1, zero, 0x10

  // osPiStartDma(0x80370C58, OS_MESG_PRI_NORMAL, OS_READ, devAddr, JUMP_TABLE_BASE, 0x10, 0x803150A0);
  lw    t0, 0x28(sp) // load level id (a0)
  sll   t0, t0, 0x4  // *= 0x10
  lui   a3, 0x7F
  ori   a3, a3, 0xC000
  addu  a3, a3, t0 // devAddr = 0x7FC000 + levelId * 0x10
  lui   t1, JUMP_TABLE_BASE >> 16
  ori   t1, t1, JUMP_TABLE_BASE & 0xFFFF
  sw    t1, 0x10(sp)
  addiu t2, zero, 0x10
  sw    t2, 0x14(sp)
  lui   t3, 0x8031
  ori   t3, 0x50A0
  sw    t3, 0x18(sp)
  lui   a0, 0x8037
  ori   a0, a0, 0x0C58
  addiu a2, zero, OS_READ
  jal   osPiStartDma
  addiu a1, zero, OS_MESG_PRI_NORMAL

  // osRecvMesg(0x803150A0, NULL, OS_MESG_BLOCK);
  lui   a0, 0x8031
  ori   a0, a0, 0x50A0
  addiu a1, zero, 0x0
  jal   osRecvMesg
  addiu a2, zero, OS_MESG_BLOCK

  // save base ROM address and length
  lui   at, JUMP_TABLE_BASE >> 16
  ori   at, at, JUMP_TABLE_BASE & 0xFFFF
  lw    t6, 0x0(at)
  sw    t6, 0x24(sp)   // store base address
  lw    t7, 0xC(at)
  sub   t7, t7, t6     // compute length
  lw    t4, 0x30(sp)
  sw    t7, 0x0(t4)    // store length = *a2

LoadLevel_Dma:
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

// fill existing code with nops to allow the new gzip code to be smaller than the original
fill 0x80256A34-pc()

// end LoadLevel
