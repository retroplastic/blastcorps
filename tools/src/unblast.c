#include <assert.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#include "blast.h"

void
print_usage()
{
  printf("Usage:\n");
  printf("Decode blast encoded file:\n");
  printf("./blast <file_path> <blast_compression_id>\n");
  printf("Decode blast encoded file. With LUT for blast 4 and 5:\n");
  printf("./blast <file_path> <blast_compression_id> <lut_file_path>\n");
}

int
main(int argc, char* argv[])
{
  if (argc < 3 || argc > 4)
  {
    print_usage();
    return EXIT_FAILURE;
  }

  // read in Blast Corps ROM
  uint8_t* rom_bytes;
  size_t size = read_file(argv[1], &rom_bytes);

  blast_t type = atoi(argv[2]);
  printf("Opening %s\n", argv[1]);

  if (argc != 4 && (type == BLAST4_IA16 || type == BLAST5_RGBA32))
  {
    printf(
      "ERROR: You need to provide a LUT file for blast4 and blast5 files.\n");
    print_usage();
    return EXIT_FAILURE;
  }

  uint8_t* lut_bytes = NULL;
  if (argc == 4)
  {
    printf("Opening LUT at %s\n", argv[3]);
    size_t lut_size = read_file(argv[3], &lut_bytes);
    if (type == BLAST4_IA16 && lut_size != 128)
    {
      printf("WARNING: Lookup table for blast4 is %ld bytes instead of 128.\n",
             lut_size);
    }
    else if (type == BLAST5_RGBA32 && lut_size != 256)
    {
      printf("WARNING: Lookup table for blast5 is %ld bytes instead of 256.\n",
             lut_size);
    }
  }

  uint8_t* decompressed_bytes = malloc(100 * size);
  int32_t decompressed_size =
    decompress_block(rom_bytes, size, type, decompressed_bytes, lut_bytes);

  // Write decompressed file
  char out_path_decompressed[512];

  if (type == BLAST0)
  {
    sprintf(out_path_decompressed, "%s.unblast0", argv[1]);
  }
  else
  {
    int32_t depth = get_type_depth(type);
    const char* format_name = get_type_format_name(type);
    sprintf(out_path_decompressed, "%s.%s%d", argv[1], format_name, depth);
  }

  printf("Writing %s\n", out_path_decompressed);

  write_file(out_path_decompressed, decompressed_bytes, decompressed_size);

  free(rom_bytes);

  return EXIT_SUCCESS;
}
