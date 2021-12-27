// SPDX-License-Identifier:Unlicense

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "blast.h"
#include "utils.h"

// list_gzips is based on SubDrag's GEDecompressor.
// It looks for zip signatures in a ROM and print results in splat yaml syntax.
void
list_gzips(uint8_t* bytes, size_t size)
{
  for (uint64_t address = 0x0; address < size; address++)
  {
    // clang-format off
    if (bytes[address]     == 0x1F &&
        bytes[address + 1] == 0x8B &&
        bytes[address + 2] == 0x08 &&
        bytes[address + 3] == 0x08)
    // clang-format on
    {
      char name[300];

      for (uint32_t pos = 0; bytes[address + 0xA + pos - 1] != 0; pos++)
      {
        name[pos] = bytes[address + 0xA + pos];
      }

      printf("  - [0x%06lX, bin, %s]\n", address, name);
    }
  }
}

#define ROM_OFFSET 0x4CE0
#define END_OFFSET 0xCCE0

void
list_blasts(uint8_t* bytes, size_t size)
{
  uint32_t last_to = 0;

  // loop through from 0x4CE0 to 0xCCE0
  for (uint32_t address = ROM_OFFSET; address < END_OFFSET; address += 8)
  {
    uint32_t start = read_u32_be(&bytes[address]);
    uint16_t compressed_size = read_u16_be(&bytes[address + 4]);
    blast_t type = read_u16_be(&bytes[address + 6]);

    assert(size >= start);

    // TODO: there are large sections of len=0, possibly LUTs for 4 & 5?
    if (compressed_size == 0)
    {
      continue;
    }

    // TODO: Figure out why 0x318CA0 has multiple entries
    // Skip all entries for 0x318CA0, but the last one
    if (start + ROM_OFFSET == 0x318CA0 && compressed_size != 256)
    {
      continue;
    }

    uint32_t from = start + ROM_OFFSET;

    if (last_to != 0 && last_to != from)
    {
      uint32_t space_size = from - last_to;
      printf("  - [0x%06X] # %d bytes\n", last_to, space_size);
    }

    res_t res = { 0, 0 };
    if (type != BLAST0)
    {
      // TODO: need to figure out where last param is set for decoders 4 and 5
      uint8_t* lut;
      switch (type)
      {
        case BLAST4_IA16:
          lut = &bytes[0x047480];
          break;
        case BLAST5_RGBA32:
          // 0x0998E0
          // 0x1E2C00
          lut = &bytes[0x152970];
          break;
        default:
          lut = NULL;
      }

      uint8_t* decompressed_bytes = malloc(100 * compressed_size);
      int32_t decompressed_size = decode_blast(&bytes[start + ROM_OFFSET],
                                               compressed_size,
                                               type,
                                               decompressed_bytes,
                                               lut);

      res = guess_resolution(type, decompressed_size);

      printf("  - [0x%06X, blast, %06X.blast%d, %d, %d, %d]\n",
             from,
             from,
             type,
             type,
             res.w,
             res.h);
    }
    else if (compressed_size == 128)
    {
      // Portential lookup table for blast4.
      printf("  - [0x%06X, bin, %06X.lut128]\n", from, from);
    }
    else if (compressed_size == 256)
    {
      // Potential lookup table for blast5.
      printf("  - [0x%06X, bin, %06X.lut256]\n", from, from);
    }
    else
    {
      printf("  - [0x%06X, blast, %06X.blast0, 0]\n", from, from);
    }

    last_to = start + ROM_OFFSET + compressed_size;
  }

  // Print last to
  printf("  - [0x%06X]\n", last_to);
}

int
main(int argc, char** argv)
{
  if (argc != 2)
  {
    printf("Usage:\n");
    printf("%s <rom_path>\n", argv[0]);
    return EXIT_FAILURE;
  }

  uint8_t* bytes;
  size_t size = read_file(argv[1], &bytes);

  list_blasts(bytes, size);
  list_gzips(bytes, size);

  free(bytes);

  return EXIT_SUCCESS;
}
