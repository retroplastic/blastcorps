#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "utils.h"

#define NUM_LEVELS 0x3C
#define JUMP_TBL_OFFSET 0x1FFC8
#define RAM_TO_ROM (0x8025615C - 0x1199C)

#define OP_MASK 0xFC000000

typedef struct
{
   unsigned start;
   unsigned end;
   char *name;
} block;

typedef struct
{
   char *block;
   char *name;
} lookup;

typedef struct
{
   unsigned scase;
   unsigned start;
   unsigned end;
} level;

static lookup names[] =
{
   {"lagp.raw", "Angel City"},
   {"valley.raw", "Outland Farm"},
   {"chimp.raw", "Simian Acres"},
   {"fact.raw", "Blackridge Works"},
   {"dip.raw", "Glory Crossing"},
   {"beetle.raw", "Shuttle Gully"},
   {"bonus1.raw", "Salvage Wharf"},
   {"bonus2.raw", "Skyfall"},
   {"bonus3.raw", "Twilight Foundry"},
   {"level9.raw", "Crystal Rift"},
   {"level10.raw", "Argent Towers"},
   {"level11.raw", "Skerries"},
   {"level12.raw", "Diamond Sands"},
   {"level13.raw", "Ebony Coast"},
   {"level14.raw", "Oyster Harbor"},
   {"level15.raw", "Carrick Point"},
   {"level16.raw", "Havoc District"},
   {"level17.raw", "Ironstone Mine"},
   {"level18.raw", "Beeton Tracks"},
   {"level19.raw", "J-Bomb"},
   {"level20.raw", "Jade Plateau"},
   {"level21.raw", "Marine Quarter"},
   {"level22.raw", "Cooter Creek"},
   {"level23.raw", "Gibbon's Gate"},
   {"level24.raw", "Baboon Catacomb"},
   {"level25.raw", "Sleek Streets"},
   {"level26.raw", "Obsidian Mile"},
   {"level27.raw", "Corvine Bluff"},
   {"level28.raw", "Sideswipe"},
   {"level29.raw", "Echo Marches"},
   {"level30.raw", "Kipling Plant"},
   {"level31.raw", "Falchion Field"},
   {"level32.raw", "Morgan Hall"},
   {"level33.raw", "Tempest City"},
   {"level34.raw", "Orion Plaza"},
   {"level35.raw", "Glander's Ranch"},
   {"level36.raw", "Dagger Pass"},
   {"level37.raw", "Geode Square"},
   {"level38.raw", "Shuttle Island"},
   {"level39.raw", "Mica Park"},
   {"level40.raw", "Moon"},
   {"level41.raw", "Cobalt Quarry"},
   {"level42.raw", "Moraine Chase"},
   {"level43.raw", "Mercury"},
   {"level44.raw", "Venus"},
   {"level45.raw", "Mars"},
   {"level46.raw", "Neptune"},
   {"level47.raw", "CMO Intro"},
   {"level48.raw", "Silver Junction"},
   {"level49.raw", "End Sequence"},
   {"level50.raw", "Shuttle Clear"},
   {"level51.raw", "Dark Heartland"},
   {"level52.raw", "Magma Peak"},
   {"level53.raw", "Thunderfist"},
   {"level54.raw", "Saline Watch"},
   {"level55.raw", "Backlash"},
   {"level56.raw", "Bison Ridge"},
   {"level57.raw", "Ember Hamlet"},
   {"level58.raw", "Cromlech Court"},
   {"level59.raw", "Lizard Island"},
};

static block blocks[] =
{
   {0x4A4120, 0x4A4F6D, "shuttle.raw"},
   {0x4A4F6D, 0x4A5660, "shuttle_dl.raw"},
   {0x4A5660, 0x4ABF91, "lagp.raw"},
   {0x4ABF91, 0x4ACC10, "lagp_dl.raw"},
   {0x4ACC10, 0x4B71AB, "chimp.raw"},
   {0x4B71AB, 0x4B8960, "chimp_dl.raw"},
   {0x4B8960, 0x4BEDE5, "valley.raw"},
   {0x4BEDE5, 0x4BFD60, "valley_dl.raw"},
   {0x4BFD60, 0x4C3247, "fact.raw"},
   {0x4C3247, 0x4C3AC0, "fact_dl.raw"},
   {0x4C3AC0, 0x4D3B76, "dip.raw"},
   {0x4D3B76, 0x4D5F90, "dip_dl.raw"},
   {0x4D5F90, 0x4E1838, "beetle.raw"},
   {0x4E1838, 0x4E2F70, "beetle_dl.raw"},
   {0x4E2F70, 0x4E48E4, "bonus1.raw"},
   {0x4E48E4, 0x4E4E80, "bonus1_dl.raw"},
   {0x4E4E80, 0x4E7458, "bonus2.raw"},
   {0x4E7458, 0x4E7C00, "bonus2_dl.raw"},
   {0x4E7C00, 0x4E8BF7, "bonus3.raw"},
   {0x4E8BF7, 0x4E8F70, "bonus3_dl.raw"},
   {0x4E8F70, 0x4F441E, "level9.raw"},
   {0x4F441E, 0x4F5C10, "level9_dl.raw"},
   {0x4F5C10, 0x4FF4C1, "level10.raw"},
   {0x4FF4C1, 0x500520, "level10_dl.raw"},
   {0x500520, 0x506D73, "level11.raw"},
   {0x506D73, 0x507E80, "level11_dl.raw"},
   {0x507E80, 0x51014A, "level12.raw"},
   {0x51014A, 0x511340, "level12_dl.raw"},
   {0x511340, 0x5213B7, "level13.raw"},
   {0x5213B7, 0x523080, "level13_dl.raw"},
   {0x523080, 0x52B974, "level14.raw"},
   {0x52B974, 0x52CD00, "level14_dl.raw"},
   {0x52CD00, 0x531885, "level15.raw"},
   {0x531885, 0x532700, "level15_dl.raw"},
   {0x532700, 0x53D32E, "level16.raw"},
   {0x53D32E, 0x53E9B0, "level16_dl.raw"},
   {0x53E9B0, 0x54909A, "level17.raw"},
   {0x54909A, 0x54A820, "level17_dl.raw"},
   {0x54A820, 0x551DAA, "level18.raw"},
   {0x551DAA, 0x552DE0, "level18_dl.raw"},
   {0x552DE0, 0x554A33, "level19.raw"},
   {0x554A33, 0x555000, "level19_dl.raw"},
   {0x555000, 0x55F205, "level20.raw"},
   {0x55F205, 0x560E90, "level20_dl.raw"},
   {0x560E90, 0x5646CB, "level21.raw"},
   {0x5646CB, 0x5652D0, "level21_dl.raw"},
   {0x5652D0, 0x56E0F7, "level22.raw"},
   {0x56E0F7, 0x56F3F0, "level22_dl.raw"},
   {0x56F3F0, 0x571E88, "level23.raw"},
   {0x571E88, 0x5721E0, "level23_dl.raw"},
   {0x5721E0, 0x573354, "level24.raw"},
   {0x573354, 0x5736E0, "level24_dl.raw"},
   {0x5736E0, 0x5795B8, "level25.raw"},
   {0x5795B8, 0x57A2C0, "level25_dl.raw"},
   {0x57A2C0, 0x58005F, "level26.raw"},
   {0x58005F, 0x580B60, "level26_dl.raw"},
   {0x580B60, 0x587CA0, "level27.raw"},
   {0x587CA0, 0x588CE0, "level27_dl.raw"},
   {0x588CE0, 0x58B53D, "level28.raw"},
   {0x58B53D, 0x58BE80, "level28_dl.raw"},
   {0x58BE80, 0x596201, "level29.raw"},
   {0x596201, 0x597B80, "level29_dl.raw"},
   {0x597B80, 0x59AEC7, "level30.raw"},
   {0x59AEC7, 0x59B7D0, "level30_dl.raw"},
   {0x59B7D0, 0x5A4451, "level31.raw"},
   {0x5A4451, 0x5A5840, "level31_dl.raw"},
   {0x5A5840, 0x5AF6C1, "level32.raw"},
   {0x5AF6C1, 0x5B0B10, "level32_dl.raw"},
   {0x5B0B10, 0x5B4E60, "level33.raw"},
   {0x5B4E60, 0x5B5A30, "level33_dl.raw"},
   {0x5B5A30, 0x5B850E, "level34.raw"},
   {0x5B850E, 0x5B8BB0, "level34_dl.raw"},
   {0x5B8BB0, 0x5C366C, "level35.raw"},
   {0x5C366C, 0x5C4C80, "level35_dl.raw"},
   {0x5C4C80, 0x5C9D2D, "level36.raw"},
   {0x5C9D2D, 0x5CA9C0, "level36_dl.raw"},
   {0x5CA9C0, 0x5CC86E, "level37.raw"},
   {0x5CC86E, 0x5CCF50, "level37_dl.raw"},
   {0x5CCF50, 0x5D07B8, "level38.raw"},
   {0x5D07B8, 0x5D1060, "level38_dl.raw"},
   {0x5D1060, 0x5DB0FC, "level39.raw"},
   {0x5DB0FC, 0x5DC830, "level39_dl.raw"},
   {0x5DC830, 0x5E5B29, "level40.raw"},
   {0x5E5B29, 0x5E6EE0, "level40_dl.raw"},
   {0x5E6EE0, 0x5EB4C8, "level41.raw"},
   {0x5EB4C8, 0x5EC800, "level41_dl.raw"},
   {0x5EC800, 0x5F2C67, "level42.raw"},
   {0x5F2C67, 0x5F3A80, "level42_dl.raw"},
   {0x5F3A80, 0x5FFF79, "level43.raw"},
   {0x5FFF79, 0x6014B0, "level43_dl.raw"},
   {0x6014B0, 0x6095C1, "level44.raw"},
   {0x6095C1, 0x60A710, "level44_dl.raw"},
   {0x60A710, 0x6126C5, "level45.raw"},
   {0x6126C5, 0x613AA0, "level45_dl.raw"},
   {0x613AA0, 0x61C88C, "level46.raw"},
   {0x61C88C, 0x61DD70, "level46_dl.raw"},
   {0x61DD70, 0x6211ED, "level47.raw"},
   {0x6211ED, 0x621AF0, "level47_dl.raw"},
   {0x621AF0, 0x625CAC, "level48.raw"},
   {0x625CAC, 0x6269E0, "level48_dl.raw"},
   {0x6269E0, 0x62F871, "level49.raw"},
   {0x62F871, 0x630C30, "level49_dl.raw"},
   {0x630C30, 0x634D35, "level50.raw"},
   {0x634D35, 0x635700, "level50_dl.raw"},
   {0x635700, 0x63BD00, "level51.raw"},
   {0x63BD00, 0x63CA10, "level51_dl.raw"},
   {0x63CA10, 0x6416D6, "level52.raw"},
   {0x6416D6, 0x641F30, "level52_dl.raw"},
   {0x641F30, 0x6440CC, "level53.raw"},
   {0x6440CC, 0x644810, "level53_dl.raw"},
   {0x644810, 0x645D55, "level54.raw"},
   {0x645D55, 0x646080, "level54_dl.raw"},
   {0x646080, 0x6470B7, "level55.raw"},
   {0x6470B7, 0x647550, "level55_dl.raw"},
   {0x647550, 0x65362E, "level56.raw"},
   {0x65362E, 0x654FC0, "level56_dl.raw"},
   {0x654FC0, 0x65F329, "level57.raw"},
   {0x65F329, 0x660950, "level57_dl.raw"},
   {0x660950, 0x66550A, "level58.raw"},
   {0x66550A, 0x665F80, "level58_dl.raw"},
   {0x665F80, 0x66BB49, "level59.raw"},
   {0x66BB49, 0x66C900, "level59_dl.raw"},
};

static level level_table[NUM_LEVELS];

int main(int argc, char *argv[])
{
   unsigned char *text;
   unsigned char *data;
   int text_size;
   int data_size;
   unsigned text_offset;
   unsigned instruction;
   unsigned rom_offset;
   unsigned opcode;
   int decoding;
   unsigned i, j, k;
   
   if (argc < 3) {
      ERROR("usage: gen_level_table <TEXT> <DATA>\n");
      return EXIT_FAILURE;
   }

   memset(level_table, 0, sizeof(level_table));
   
   text_size = read_file(argv[1], &text);
   if (text_size <= 0) {
      ERROR("Error opening \"%s\"\n", argv[1]);
      return EXIT_FAILURE;
   }
   
   data_size = read_file(argv[2], &data);
   if (data_size <= 0) {
      ERROR("Error opening \"%s\"\n", argv[2]);
      return EXIT_FAILURE;
   }
   
   // read jump table
   for (i = 0; i < NUM_LEVELS; i++) {
      level_table[i].scase = read_u32_be(&data[JUMP_TBL_OFFSET+4*i]);
   }
   
   // read offsets from code
   rom_offset = 0;
   for (i = 0; i < NUM_LEVELS; i++) {
      text_offset = level_table[i].scase - RAM_TO_ROM;
      decoding = 1;
      while (decoding) {
         assert(text_offset < text_size);
         instruction = read_u32_be(&text[text_offset]);
         opcode = instruction & OP_MASK;
         switch (opcode) {
            case 0x3C000000: // LUI
               rom_offset = (instruction & 0xFFFF) << 16;
               break;
            case 0x24000000: // ADDIU
               rom_offset += (instruction & 0xFFFF);
               if (instruction & 0x8000) {
                  rom_offset -= 0x10000;
               }
               if (decoding == 1) level_table[i].start = rom_offset;
               else level_table[i].end = rom_offset;
               decoding++;
               break;
            case 0xAC000000: // SW
               decoding = 0;
               break;
            default:
               break;
         }
         text_offset += 4;
      }
   }
   
   // print table
   for (i = 0; i < NUM_LEVELS; i++) {
      level *l = &level_table[i];
      printf("%2d: %08X %06X %06X", i, l->scase, l->start, l->end);
      for (j = 0; j < DIM(blocks); j++) {
         if (blocks[j].start == l->start) {
            printf(" %-11s", blocks[j].name);
            for (k = 0; k < DIM(names); k++) {
               if (0 == strcmp(blocks[j].name, names[k].block)) {
                  printf(" %s", names[k].name);
                  break;
               }
            }
            break;
         }
      }
      printf("\n");
   }
   
   free(text);
   free(data);
   
   return EXIT_SUCCESS;
}