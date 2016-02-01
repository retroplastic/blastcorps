.org 0x119BC

  sll   t6, t6, 0x3      //
  lui   at, 0x80        // %hi(0x7FA000)
  addu  at, at, t6    
  lw    t6, 0xA000(at) // %lo(0x7FA000)
  sw    t6, 0x24(sp)   //
  lw    t7, 0xA004(at) // %lo(0x7FA004)
  sub   t7, t7, t6    //
  lw    t4, 0x30(sp)   //
  sw    t7, 0x0(t4)       //
  b     0x00012240 //
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop