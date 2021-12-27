// SPDX-License-Identifier:Unlicense

// This tiny tool is based on SubDrag's GEDecompressor.
// It looks for zip signatures in a ROM and print results in splat yaml syntax.

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

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

  list_gzips(bytes, size);

  free(bytes);

  return EXIT_SUCCESS;
}
