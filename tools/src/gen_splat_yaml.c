// SPDX-License-Identifier:Unlicense

// This tiny tool is based on SubDrag's GEDecompressor.
// It looks for zip signatures in a ROM and print results in splat yaml syntax.

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char** argv)
{
  if (argc != 2)
  {
    printf("Usage:\n");
    printf("%s <rom_path>\n", argv[0]);
    return EXIT_FAILURE;
  }

  FILE* rom = fopen(argv[1], "rb");
  if (rom == NULL)
  {
    printf("Invalid ROM\n");
    return EXIT_FAILURE;
  }

  fseek(rom, 0, SEEK_END);
  size_t size = ftell(rom);
  rewind(rom);

  uint8_t* bytes = calloc(size, sizeof(uint8_t));
  fread(bytes, sizeof(uint8_t), size, rom);

  if (ferror(rom))
  {
    perror("Error reading rom file.");
    return EXIT_FAILURE;
  }

  fclose(rom);

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

  free(bytes);

  return EXIT_SUCCESS;
}
