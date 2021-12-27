#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
  BLAST0 = 0,
  BLAST1_RGBA16,
  BLAST2_RGBA32,
  BLAST3_IA8,
  BLAST4_IA16,
  BLAST5_RGBA32,
  BLAST6_IA8
} blast_t;

typedef struct
{
  uint32_t w;
  uint32_t h;
} res_t;

int32_t
decode_blast(uint8_t* src, uint32_t size, blast_t type, uint8_t* result_bytes);

int32_t
decode_blast_lookup(uint8_t* src,
                    uint32_t size,
                    blast_t type,
                    uint8_t* result_bytes,
                    uint8_t* lut);

// This cannot be reliably determined by the data and needs to be
// tracked manually in the splat yaml.
res_t
guess_resolution(blast_t type, int32_t size);

const char*
get_type_format_name(blast_t type);

int32_t
get_type_depth(blast_t type);

bool
convert_to_png(char* fname, uint16_t len, blast_t type);
