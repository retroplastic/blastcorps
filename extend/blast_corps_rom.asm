// Blast Corps (U) (V1.1).z64 patch to use ROM lookup table
// Updated: 2016 Feb 15
// By: queueRAM
// For updates, check: http://origami64.net/showthread.php?tid=334&pid=2566

// Use with N64 version of bass: https://github.com/ARM9/bass
// > bass -o "Blast Corps (U) (V1.1).z64" blast_corps_rom.asm

arch n64.cpu
endian msb

include "mips4300.inc"

// TODO: update end offset of code
// origin 0x2A4C
// addiu a3, a3, 0x3acc

// overwrite old chimp level
origin 0x4ACC10
fill 0x4B8960-0x4ACC10

// new code
origin 0x787FD0
insert "bin/hd_code_text.raw.gz"
insert "bin/hd_code_data.raw.gz"
// ensure new code isn't too large
if pc() > 0x7E3AD0 {
   error "code + data > 0x7E3AD0"
}

fill 0x7E3AD0-pc()

// Level LUT
origin 0x7FC000
dw   chimp_start,   chimp_dl,   chimp_dl,   chimp_end // 0x4ACC10, 0x4B71AB, 0x4B71AB, 0x4B8960
dw    lagp_start,    lagp_dl,    lagp_dl,    lagp_end // 0x4A5660, 0x4ABF91, 0x4ABF91, 0x4ACC10
dw  valley_start,  valley_dl,  valley_dl,  valley_end // 0x4B8960, 0x4BEDE5, 0x4BEDE5, 0x4BFD60
dw    fact_start,    fact_dl,    fact_dl,    fact_end // 0x4BFD60, 0x4C3247, 0x4C3247, 0x4C3AC0
dw     dip_start,     dip_dl,     dip_dl,     dip_end // 0x4C3AC0, 0x4D3B76, 0x4D3B76, 0x4D5F90
dw  beetle_start,  beetle_dl,  beetle_dl,  beetle_end // 0x4D5F90, 0x4E1838, 0x4E1838, 0x4E2F70
dw  bonus1_start,  bonus1_dl,  bonus1_dl,  bonus1_end // 0x4E2F70, 0x4E48E4, 0x4E48E4, 0x4E4E80
dw  bonus2_start,  bonus2_dl,  bonus2_dl,  bonus2_end // 0x4E4E80, 0x4E7458, 0x4E7458, 0x4E7C00
dw  bonus3_start,  bonus3_dl,  bonus3_dl,  bonus3_end // 0x4E7C00, 0x4E8BF7, 0x4E8BF7, 0x4E8F70
dw  level9_start,  level9_dl,  level9_dl,  level9_end // 0x4E8F70, 0x4F441E, 0x4F441E, 0x4F5C10
dw level10_start, level10_dl, level10_dl, level10_end // 0x4F5C10, 0x4FF4C1, 0x4FF4C1, 0x500520
dw level11_start, level11_dl, level11_dl, level11_end // 0x500520, 0x506D73, 0x506D73, 0x507E80
dw level12_start, level12_dl, level12_dl, level12_end // 0x507E80, 0x51014A, 0x51014A, 0x511340
dw level13_start, level13_dl, level13_dl, level13_end // 0x511340, 0x5213B7, 0x5213B7, 0x523080
dw level14_start, level14_dl, level14_dl, level14_end // 0x523080, 0x52B974, 0x52B974, 0x52CD00
dw level15_start, level15_dl, level15_dl, level15_end // 0x52CD00, 0x531885, 0x531885, 0x532700
dw level16_start, level16_dl, level16_dl, level16_end // 0x532700, 0x53D32E, 0x53D32E, 0x53E9B0
dw level17_start, level17_dl, level17_dl, level17_end // 0x53E9B0, 0x54909A, 0x54909A, 0x54A820
dw level18_start, level18_dl, level18_dl, level18_end // 0x54A820, 0x551DAA, 0x551DAA, 0x552DE0
dw level19_start, level19_dl, level19_dl, level19_end // 0x552DE0, 0x554A33, 0x554A33, 0x555000
dw level20_start, level20_dl, level20_dl, level20_end // 0x555000, 0x55F205, 0x55F205, 0x560E90
dw level21_start, level21_dl, level21_dl, level21_end // 0x560E90, 0x5646CB, 0x5646CB, 0x5652D0
dw level22_start, level22_dl, level22_dl, level22_end // 0x5652D0, 0x56E0F7, 0x56E0F7, 0x56F3F0
dw level23_start, level23_dl, level23_dl, level23_end // 0x56F3F0, 0x571E88, 0x571E88, 0x5721E0
dw level24_start, level24_dl, level24_dl, level24_end // 0x5721E0, 0x573354, 0x573354, 0x5736E0
dw level25_start, level25_dl, level25_dl, level25_end // 0x5736E0, 0x5795B8, 0x5795B8, 0x57A2C0
dw level26_start, level26_dl, level26_dl, level26_end // 0x57A2C0, 0x58005F, 0x58005F, 0x580B60
dw level27_start, level27_dl, level27_dl, level27_end // 0x580B60, 0x587CA0, 0x587CA0, 0x588CE0
dw level28_start, level28_dl, level28_dl, level28_end // 0x588CE0, 0x58B53D, 0x58B53D, 0x58BE80
dw level29_start, level29_dl, level29_dl, level29_end // 0x58BE80, 0x596201, 0x596201, 0x597B80
dw level30_start, level30_dl, level30_dl, level30_end // 0x597B80, 0x59AEC7, 0x59AEC7, 0x59B7D0
dw level31_start, level31_dl, level31_dl, level31_end // 0x59B7D0, 0x5A4451, 0x5A4451, 0x5A5840
dw level32_start, level32_dl, level32_dl, level32_end // 0x5A5840, 0x5AF6C1, 0x5AF6C1, 0x5B0B10
dw level33_start, level33_dl, level33_dl, level33_end // 0x5B0B10, 0x5B4E60, 0x5B4E60, 0x5B5A30
dw level34_start, level34_dl, level34_dl, level34_end // 0x5B5A30, 0x5B850E, 0x5B850E, 0x5B8BB0
dw level35_start, level35_dl, level35_dl, level35_end // 0x5B8BB0, 0x5C366C, 0x5C366C, 0x5C4C80
dw level36_start, level36_dl, level36_dl, level36_end // 0x5C4C80, 0x5C9D2D, 0x5C9D2D, 0x5CA9C0
dw level37_start, level37_dl, level37_dl, level37_end // 0x5CA9C0, 0x5CC86E, 0x5CC86E, 0x5CCF50
dw level38_start, level38_dl, level38_dl, level38_end // 0x5CCF50, 0x5D07B8, 0x5D07B8, 0x5D1060
dw level39_start, level39_dl, level39_dl, level39_end // 0x5D1060, 0x5DB0FC, 0x5DB0FC, 0x5DC830
dw level40_start, level40_dl, level40_dl, level40_end // 0x5DC830, 0x5E5B29, 0x5E5B29, 0x5E6EE0
dw level41_start, level41_dl, level41_dl, level41_end // 0x5E6EE0, 0x5EB4C8, 0x5EB4C8, 0x5EC800
dw level42_start, level42_dl, level42_dl, level42_end // 0x5EC800, 0x5F2C67, 0x5F2C67, 0x5F3A80
dw level43_start, level43_dl, level43_dl, level43_end // 0x5F3A80, 0x5FFF79, 0x5FFF79, 0x6014B0
dw level44_start, level44_dl, level44_dl, level44_end // 0x6014B0, 0x6095C1, 0x6095C1, 0x60A710
dw level45_start, level45_dl, level45_dl, level45_end // 0x60A710, 0x6126C5, 0x6126C5, 0x613AA0
dw level46_start, level46_dl, level46_dl, level46_end // 0x613AA0, 0x61C88C, 0x61C88C, 0x61DD70
dw level47_start, level47_dl, level47_dl, level47_end // 0x61DD70, 0x6211ED, 0x6211ED, 0x621AF0
dw level48_start, level48_dl, level48_dl, level48_end // 0x621AF0, 0x625CAC, 0x625CAC, 0x6269E0
dw level49_start, level49_dl, level49_dl, level49_end // 0x6269E0, 0x62F871, 0x62F871, 0x630C30
dw level50_start, level50_dl, level50_dl, level50_end // 0x630C30, 0x634D35, 0x634D35, 0x635700
dw level51_start, level51_dl, level51_dl, level51_end // 0x635700, 0x63BD00, 0x63BD00, 0x63CA10
dw level52_start, level52_dl, level52_dl, level52_end // 0x63CA10, 0x6416D6, 0x6416D6, 0x641F30
dw level53_start, level53_dl, level53_dl, level53_end // 0x641F30, 0x6440CC, 0x6440CC, 0x644810
dw level54_start, level54_dl, level54_dl, level54_end // 0x644810, 0x645D55, 0x645D55, 0x646080
dw level55_start, level55_dl, level55_dl, level55_end // 0x646080, 0x6470B7, 0x6470B7, 0x647550
dw level56_start, level56_dl, level56_dl, level56_end // 0x647550, 0x65362E, 0x65362E, 0x654FC0
dw level57_start, level57_dl, level57_dl, level57_end // 0x654FC0, 0x65F329, 0x65F329, 0x660950
dw level58_start, level58_dl, level58_dl, level58_end // 0x660950, 0x66550A, 0x66550A, 0x665F80
dw level59_start, level59_dl, level59_dl, level59_end // 0x665F80, 0x66BB49, 0x66BB49, 0x66C900


// append level at end of ROM
origin 0x800000

chimp_start:
insert "levels/chimp.raw.gz"
chimp_dl:
insert "levels/chimp_dl.raw.gz"
chimp_end:

align(0x10)

lagp_start:
insert "levels/lagp.raw.gz"
lagp_dl:
insert "levels/lagp_dl.raw.gz"
lagp_end:

align(0x10)

valley_start:
insert "levels/valley.raw.gz"
valley_dl:
insert "levels/valley_dl.raw.gz"
valley_end:

align(0x10)

fact_start:
insert "levels/fact.raw.gz"
fact_dl:
insert "levels/fact_dl.raw.gz"
fact_end:

align(0x10)

dip_start:
insert "levels/dip.raw.gz"
dip_dl:
insert "levels/dip_dl.raw.gz"
dip_end:

align(0x10)

beetle_start:
insert "levels/beetle.raw.gz"
beetle_dl:
insert "levels/beetle_dl.raw.gz"
beetle_end:

align(0x10)

bonus1_start:
insert "levels/bonus1.raw.gz"
bonus1_dl:
insert "levels/bonus1_dl.raw.gz"
bonus1_end:

align(0x10)

bonus2_start:
insert "levels/bonus2.raw.gz"
bonus2_dl:
insert "levels/bonus2_dl.raw.gz"
bonus2_end:

align(0x10)

bonus3_start:
insert "levels/bonus3.raw.gz"
bonus3_dl:
insert "levels/bonus3_dl.raw.gz"
bonus3_end:

align(0x10)

level9_start:
insert "levels/level9.raw.gz"
level9_dl:
insert "levels/level9_dl.raw.gz"
level9_end:

align(0x10)

level10_start:
insert "levels/level10.raw.gz"
level10_dl:
insert "levels/level10_dl.raw.gz"
level10_end:

align(0x10)

level11_start:
insert "levels/level11.raw.gz"
level11_dl:
insert "levels/level11_dl.raw.gz"
level11_end:

align(0x10)

level12_start:
insert "levels/level12.raw.gz"
level12_dl:
insert "levels/level12_dl.raw.gz"
level12_end:

align(0x10)

level13_start:
insert "levels/level13.raw.gz"
level13_dl:
insert "levels/level13_dl.raw.gz"
level13_end:

align(0x10)

level14_start:
insert "levels/level14.raw.gz"
level14_dl:
insert "levels/level14_dl.raw.gz"
level14_end:

align(0x10)

level15_start:
insert "levels/level15.raw.gz"
level15_dl:
insert "levels/level15_dl.raw.gz"
level15_end:

align(0x10)

level16_start:
insert "levels/level16.raw.gz"
level16_dl:
insert "levels/level16_dl.raw.gz"
level16_end:

align(0x10)

level17_start:
insert "levels/level17.raw.gz"
level17_dl:
insert "levels/level17_dl.raw.gz"
level17_end:

align(0x10)

level18_start:
insert "levels/level18.raw.gz"
level18_dl:
insert "levels/level18_dl.raw.gz"
level18_end:

align(0x10)

level19_start:
insert "levels/level19.raw.gz"
level19_dl:
insert "levels/level19_dl.raw.gz"
level19_end:

align(0x10)

level20_start:
insert "levels/level20.raw.gz"
level20_dl:
insert "levels/level20_dl.raw.gz"
level20_end:

align(0x10)

level21_start:
insert "levels/level21.raw.gz"
level21_dl:
insert "levels/level21_dl.raw.gz"
level21_end:

align(0x10)

level22_start:
insert "levels/level22.raw.gz"
level22_dl:
insert "levels/level22_dl.raw.gz"
level22_end:

align(0x10)

level23_start:
insert "levels/level23.raw.gz"
level23_dl:
insert "levels/level23_dl.raw.gz"
level23_end:

align(0x10)

level24_start:
insert "levels/level24.raw.gz"
level24_dl:
insert "levels/level24_dl.raw.gz"
level24_end:

align(0x10)

level25_start:
insert "levels/level25.raw.gz"
level25_dl:
insert "levels/level25_dl.raw.gz"
level25_end:

align(0x10)

level26_start:
insert "levels/level26.raw.gz"
level26_dl:
insert "levels/level26_dl.raw.gz"
level26_end:

align(0x10)

level27_start:
insert "levels/level27.raw.gz"
level27_dl:
insert "levels/level27_dl.raw.gz"
level27_end:

align(0x10)

level28_start:
insert "levels/level28.raw.gz"
level28_dl:
insert "levels/level28_dl.raw.gz"
level28_end:

align(0x10)

level29_start:
insert "levels/level29.raw.gz"
level29_dl:
insert "levels/level29_dl.raw.gz"
level29_end:

align(0x10)

level30_start:
insert "levels/level30.raw.gz"
level30_dl:
insert "levels/level30_dl.raw.gz"
level30_end:

align(0x10)

level31_start:
insert "levels/level31.raw.gz"
level31_dl:
insert "levels/level31_dl.raw.gz"
level31_end:

align(0x10)

level32_start:
insert "levels/level32.raw.gz"
level32_dl:
insert "levels/level32_dl.raw.gz"
level32_end:

align(0x10)

level33_start:
insert "levels/level33.raw.gz"
level33_dl:
insert "levels/level33_dl.raw.gz"
level33_end:

align(0x10)

level34_start:
insert "levels/level34.raw.gz"
level34_dl:
insert "levels/level34_dl.raw.gz"
level34_end:

align(0x10)

level35_start:
insert "levels/level35.raw.gz"
level35_dl:
insert "levels/level35_dl.raw.gz"
level35_end:

align(0x10)

level36_start:
insert "levels/level36.raw.gz"
level36_dl:
insert "levels/level36_dl.raw.gz"
level36_end:

align(0x10)

level37_start:
insert "levels/level37.raw.gz"
level37_dl:
insert "levels/level37_dl.raw.gz"
level37_end:

align(0x10)

level38_start:
insert "levels/level38.raw.gz"
level38_dl:
insert "levels/level38_dl.raw.gz"
level38_end:

align(0x10)

level39_start:
insert "levels/level39.raw.gz"
level39_dl:
insert "levels/level39_dl.raw.gz"
level39_end:

align(0x10)

level40_start:
insert "levels/level40.raw.gz"
level40_dl:
insert "levels/level40_dl.raw.gz"
level40_end:

align(0x10)

level41_start:
insert "levels/level41.raw.gz"
level41_dl:
insert "levels/level41_dl.raw.gz"
level41_end:

align(0x10)

level42_start:
insert "levels/level42.raw.gz"
level42_dl:
insert "levels/level42_dl.raw.gz"
level42_end:

align(0x10)

level43_start:
insert "levels/level43.raw.gz"
level43_dl:
insert "levels/level43_dl.raw.gz"
level43_end:

align(0x10)

level44_start:
insert "levels/level44.raw.gz"
level44_dl:
insert "levels/level44_dl.raw.gz"
level44_end:

align(0x10)

level45_start:
insert "levels/level45.raw.gz"
level45_dl:
insert "levels/level45_dl.raw.gz"
level45_end:

align(0x10)

level46_start:
insert "levels/level46.raw.gz"
level46_dl:
insert "levels/level46_dl.raw.gz"
level46_end:

align(0x10)

level47_start:
insert "levels/level47.raw.gz"
level47_dl:
insert "levels/level47_dl.raw.gz"
level47_end:

align(0x10)

level48_start:
insert "levels/level48.raw.gz"
level48_dl:
insert "levels/level48_dl.raw.gz"
level48_end:

align(0x10)

level49_start:
insert "levels/level49.raw.gz"
level49_dl:
insert "levels/level49_dl.raw.gz"
level49_end:

align(0x10)

level50_start:
insert "levels/level50.raw.gz"
level50_dl:
insert "levels/level50_dl.raw.gz"
level50_end:

align(0x10)

level51_start:
insert "levels/level51.raw.gz"
level51_dl:
insert "levels/level51_dl.raw.gz"
level51_end:

align(0x10)

level52_start:
insert "levels/level52.raw.gz"
level52_dl:
insert "levels/level52_dl.raw.gz"
level52_end:

align(0x10)

level53_start:
insert "levels/level53.raw.gz"
level53_dl:
insert "levels/level53_dl.raw.gz"
level53_end:

align(0x10)

level54_start:
insert "levels/level54.raw.gz"
level54_dl:
insert "levels/level54_dl.raw.gz"
level54_end:

align(0x10)

level55_start:
insert "levels/level55.raw.gz"
level55_dl:
insert "levels/level55_dl.raw.gz"
level55_end:

align(0x10)

level56_start:
insert "levels/level56.raw.gz"
level56_dl:
insert "levels/level56_dl.raw.gz"
level56_end:

align(0x10)

level57_start:
insert "levels/level57.raw.gz"
level57_dl:
insert "levels/level57_dl.raw.gz"
level57_end:

align(0x10)

level58_start:
insert "levels/level58.raw.gz"
level58_dl:
insert "levels/level58_dl.raw.gz"
level58_end:

align(0x10)

level59_start:
insert "levels/level59.raw.gz"
level59_dl:
insert "levels/level59_dl.raw.gz"
level59_end:

fill 0xC00000-origin()
