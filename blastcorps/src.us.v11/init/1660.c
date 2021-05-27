#include "common.h"
#include <ultra64.h>

extern s32 D_80222840;
extern s32 D_802229E0;
extern u8 *D_802229F0;
extern s32 D_802229F4;
extern s32 D_80222A08;
extern s32 D_80222A0C;
extern s32 D_80222A18;
extern s32 D_80222A1C;
extern s32 D_80222A20;

void func_802206D0(void);
s32  func_8022043C(void);


void func_80220360(s32 *arg0, s32 *arg1, s32 arg2) {
    D_802229F0 = *arg0;
    D_802229F4 = *arg1;
    D_802229E0 = arg2;

    func_802206D0();

    if (*(D_802229F0 + D_80222A1C) != 0x1F) {
        D_80222A1C += 1;
    }
    D_80222840 = func_8022043C();
    if (D_80222840 >= 0) {
        func_80220268();
        *arg0 += D_80222A1C;
        *arg1 += D_80222A20;
    }
}

#pragma GLOBAL_ASM("asm/nonmatchings/init/1660/func_8022043C.s")

#pragma GLOBAL_ASM("asm/nonmatchings/init/1660/func_8022069C.s")
// u32 func_8022069C(u32 arg0, s32 arg1) {
//     s32 temp_a1;
//     u32 temp_t8;
//     u32 phi_a2;
//
//     phi_a2 = 0U;
// loop_1:
//     temp_t8 = (phi_a2 | (arg0 & 1)) << 1;
//     phi_a2 = temp_t8;
//     arg1 = arg1 - 1;
//     arg0 = arg0 >> 1;
//     if (arg1 > 0) {
//         goto loop_1;
//     }
//     return temp_t8 >> 1;
// }

void func_802206D0(void) {
    D_80222A20 = 0;
    D_80222A1C = 0;
    D_80222A18 = 0;
    D_80222A0C = 0;
    D_80222A08 = 0;
}
