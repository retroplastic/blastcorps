#include <assert.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#include "blast.h"

#define ROM_OFFSET 0x4CE0
#define END_OFFSET 0xCCE0

void
decompress_rom(const char* rom_path, uint8_t* rom_bytes, size_t rom_size)
{
  char rom_dir_path[300];

  strcpy(rom_dir_path,rom_path);
  dirname(rom_dir_path);

  // loop through from 0x4CE0 to 0xCCE0
  for (uint32_t address = ROM_OFFSET; address < END_OFFSET; address += 8)
  {
    uint32_t start = read_u32_be(&rom_bytes[address]);
    uint16_t compressed_size = read_u16_be(&rom_bytes[address + 4]);
    blast_t type = read_u16_be(&rom_bytes[address + 6]);

    assert(rom_size >= start);

    // TODO: there are large sections of len=0, possibly LUTs for 4 & 5?
    if (compressed_size == 0)
    {
      continue;
    }

    // Write compressed file
    char out_path_compressed[512];
    sprintf(out_path_compressed,
            "%s/%06X.blast%d",
            rom_dir_path,
            start + ROM_OFFSET,
            type);
    write_file(
      out_path_compressed, &rom_bytes[start + ROM_OFFSET], compressed_size);

    int32_t decompressed_size = 0;
    uint8_t* decompressed_bytes = malloc(100 * compressed_size);
    if (type == BLAST4_IA16 || type == BLAST5_RGBA32)
    {
      // TODO: need to figure out where last param is set for decoders 4 and 5
      uint8_t* lut;
      switch (type)
      {
        case BLAST4_IA16:
          lut = &rom_bytes[0x047480];
          break;
        case BLAST5_RGBA32:
          // 0x0998E0
          // 0x1E2C00
          lut = &rom_bytes[0x152970];
          break;
        default:
          lut = NULL;
      }
      decompressed_size = decode_blast_lookup(&rom_bytes[start + ROM_OFFSET],
                                              compressed_size,
                                              type,
                                              decompressed_bytes,
                                              lut);
    }
    else
    {
      decompressed_size = decode_blast(&rom_bytes[start + ROM_OFFSET],
                                       compressed_size,
                                       type,
                                       decompressed_bytes);
    }

    // Write decompressed file
    char out_path_decompressed[512];

    if (type == BLAST0)
    {
      sprintf(out_path_decompressed,
              "%s/%06X.unblast0",
              rom_dir_path,
              start + ROM_OFFSET);

      write_file(out_path_decompressed, decompressed_bytes, decompressed_size);

      printf("[0x%06X, 0x%06X] blast0 %5d bytes\n",
             start + ROM_OFFSET,
             start + ROM_OFFSET + compressed_size,
             compressed_size);
      continue;
    }

    int32_t depth = get_type_depth(type);
    const char* format_name = get_type_format_name(type);

    sprintf(out_path_decompressed,
            "%s/%06X.%s%d",
            rom_dir_path,
            start + ROM_OFFSET,
            format_name,
            depth);

    write_file(out_path_decompressed, decompressed_bytes, decompressed_size);

    res_t res = guess_resolution(type, decompressed_size);

    char format_str[16];
    sprintf(format_str, "(%s%d)", format_name, depth);

    printf("[0x%06X, 0x%06X] blast%d %8s %2dx%2d %4d -> %4d bytes\n",
           start + ROM_OFFSET,
           start + ROM_OFFSET + compressed_size,
           type,
           format_str,
           res.w,
           res.h,
           compressed_size,
           decompressed_size);

    // Write PNG
    convert_to_png(out_path_decompressed, decompressed_size, type);
  }
}

void
print_usage(const char* exe_name)
{
  printf("Usage:\n");
  printf("Unpack whole rom:\n");
  printf("%s <rom_path>\n", exe_name);
}

int
main(int argc, char* argv[])
{
  if (argc != 2)
  {
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }

  // read in Blast Corps ROM
  uint8_t* rom_bytes;
  size_t size = read_file(argv[1], &rom_bytes);

  decompress_rom(argv[1], rom_bytes, size);

  free(rom_bytes);

  return EXIT_SUCCESS;
}
