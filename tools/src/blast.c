#include <assert.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "n64graphics.h"
#include "utils.h"

#define ROM_OFFSET 0x4CE0
#define END_OFFSET 0xCCE0

// 802A5E10 (061650)
// just a memcpy from a0 to a3
int32_t
decode_block0(uint8_t* in, int32_t length, uint8_t* out)
{
  int32_t ret_len = length;
  length >>= 3; // a1 - gets number of dwords
  while (length != 0)
  {
    int32_t i;
    for (i = 0; i < 8; i++)
    {
      out[i] = in[i];
    }
    in += 8;
    out += 8;
    length--;
  }
  return ret_len;
}

// 802A5AE0 (061320)
int32_t
decode_block1(uint8_t* in, int32_t length, uint8_t* out)
{
  uint16_t t0, t1, t3;
  uint8_t* t2;
  int32_t len = 0;
  while (length != 0)
  {
    t0 = read_u16_be(in); // a0
    in += 2;              // a0
    if ((t0 & 0x8000) == 0)
    {
      t1 = (t0 & 0xFFC0) << 1;
      t0 &= 0x3F;
      t0 = t0 | t1;
      write_u16_be(out, t0);
      out += 2; // a3
      len += 2;
      length -= 2; // a1
    }
    else
    {
      t1 = t0 & 0x1F;          // lookback length
      t0 = (t0 & 0x7FFF) >> 5; // lookback offset
      length -= 2;             // a1
      t2 = out - t0;           // t2 - lookback pointer from current out
      while (t1 != 0)
      {
        t3 = read_u16_be(t2);
        t2 += 2;
        out += 2; // a3
        len += 2;
        t1 -= 1;
        write_u16_be(out - 2, t3);
      }
    }
  }
  return len;
}

// 802A5B90 (0613D0)
int32_t
decode_block2(uint8_t* in, int32_t length, uint8_t* out)
{
  uint8_t* look;
  uint16_t t0;
  uint32_t t1, t2, t3;
  int32_t len = 0;
  while (length != 0)
  {
    t0 = read_u16_be(in);
    in += 2;
    if ((t0 & 0x8000) == 0)
    { // t0 >= 0
      t1 = t0 & 0x7800;
      t2 = t0 & 0x0780;
      t1 <<= 17; // 0x11
      t2 <<= 13; // 0xD;
      t1 |= t2;
      t2 = t0 & 0x78;
      t2 <<= 9;
      t1 |= t2;
      t2 = t0 & 7;
      t2 <<= 5;
      t1 |= t2;
      write_u32_be(out, t1);
      out += 4;
      length -= 2;
      len += 4;
    }
    else
    {
      t1 = t0 & 0x1f;
      t0 &= 0x7FE0;
      t0 >>= 4;
      length -= 2;
      look = out - t0; // t2
      while (t1 != 0)
      {
        t3 = read_u32_be(look); // lw t2
        write_u32_be(out, t3);
        look += 4; // t2
        t1 -= 1;
        out += 4;
        len += 4;
      }
    }
  }
  return len;
}

// 802A5A2C (06126C)
int32_t
decode_block3(uint8_t* in, int32_t length, uint8_t* out)
{
  uint16_t t0, t1, t3;
  uint8_t* t2;
  int32_t len = 0;
  while (length != 0)
  {
    t0 = read_u16_be(in);
    in += 2;
    if ((0x8000 & t0) == 0)
    {
      t1 = t0 >> 8;
      t1 <<= 1;
      *out = (uint8_t)t1; // sb
      t1 = t0 & 0xFF;
      t1 <<= 1;
      *(out + 1) = (uint8_t)t1; // sb
      out += 2;
      length -= 2;
      len += 2;
    }
    else
    {
      t1 = t0 & 0x1F;
      t0 &= 0x7FFF;
      t0 >>= 5;
      length -= 2;
      t2 = out - t0;
      while (t1 != 0)
      {
        t3 = read_u16_be(t2);
        t2 += 2;
        t1 -= 1;
        write_u16_be(out, t3);
        out += 2;
        len += 2;
      }
    }
  }
  return len;
}

// 802A5C5C (06149C)
int32_t
decode_block4(uint8_t* in, int32_t length, uint8_t* out, uint8_t* lut)
{
  uint8_t* look;
  uint32_t t3;
  uint16_t t0, t1, t2;
  int32_t len = 0;
  while (length != 0)
  {
    t0 = read_u16_be(in);
    in += 2;
    if ((t0 & 0x8000) == 0)
    {
      t1 = t0 >> 8;
      t2 = t1 & 0xFE;
      look =
        lut + t2; // t2 += t4; // t4 set in proc_802A57DC: lw    $t4, 0xc($a0)
      t2 = read_u16_be(look);
      t1 &= 1;
      t2 <<= 1;
      t1 |= t2;
      write_u16_be(out, t1);
      out += 2;
      t1 = t0 & 0xFE;
      look = lut + t1;
      t1 = read_u16_be(look);
      t0 &= 1;
      length -= 2;
      t1 <<= 1;
      t1 |= t0;
      write_u16_be(out, t1);
      out += 2;
      len += 4;
    }
    else
    {
      t1 = t0 & 0x1F;
      t0 &= 0x7FE0;
      t0 >>= 4;
      length -= 2;
      look = out - t0;
      while (t1 != 0)
      {
        t3 = read_u32_be(look);
        look += 4;
        t1 -= 1;
        write_u32_be(out, t3);
        out += 4;
        len += 4;
      }
    }
  }
  return len;
}

// 802A5D34 (061574)
int32_t
decode_block5(uint8_t* in, int32_t length, uint8_t* out, uint8_t* lut)
{
  uint8_t* tmp;
  uint16_t t0, t1;
  uint32_t t2, t3;
  int32_t len = 0;
  while (length != 0)
  {
    t0 = read_u16_be(in);
    in += 2;
    if ((t0 & 0x8000) == 0)
    { // bltz
      t1 = t0 >> 4;
      t1 = t1 << 1;
      tmp = t1 + lut; // t1 += t4
      t1 = read_u16_be(tmp);
      t0 &= 0xF;
      t0 <<= 4;
      t2 = t1 & 0x7C00;
      t3 = t1 & 0x03E0;
      t2 <<= 17; // 0x11
      t3 <<= 14; // 0xe
      t2 |= t3;
      t3 = t1 & 0x1F;
      t3 <<= 11; // 0xb
      t2 |= t3;
      t2 |= t0;
      write_u32_be(out, t2);
      out += 4;
      length -= 2;
      len += 4;
    }
    else
    {
      t1 = t0 & 0x1F;
      t0 &= 0x7FE0;
      t0 >>= 4;
      length -= 2;
      tmp = out - t0; // t2
      while (t1 != 0)
      {
        t3 = read_u32_be(tmp); // t2
        tmp += 4;              // t2
        t1 -= 1;
        write_u32_be(out, t3);
        out += 4;
        len += 4;
      }
    }
  }
  return len;
}

// 802A5958 (061198)
int32_t
decode_block6(uint8_t* in, int32_t length, uint8_t* out)
{
  uint16_t t0, t1, t3;
  int32_t len = 0;
  // .Lproc_802A5958_20: # 802A5978
  while (length != 0)
  {
    t0 = read_u16_be(in);
    in += 2;
    if ((0x8000 & t0) == 0)
    {
      uint16_t t2;
      t1 = t0 >> 8;
      t2 = t1 & 0x38;
      t1 = t1 & 0x07;
      t2 <<= 2;
      t1 <<= 1;
      t1 |= t2;
      *out = t1; // sb
      t1 = t0 & 0xFF;
      t2 = t1 & 0x38;
      t1 = t1 & 0x07;
      t2 <<= 2;
      t1 <<= 1;
      t1 |= t2;
      *(out + 1) = t1;
      out += 2;
      length -= 2;
      len += 2;
    }
    else
    {
      uint8_t* t2;
      t1 = t0 & 0x1F;
      t0 = t0 & 0x7FFF;
      t0 >>= 5;
      length -= 2;
      t2 = out - t0;
      while (t1 != 0)
      {
        t3 = read_u16_be(t2);
        t2 += 2;
        t1 -= 1;
        write_u16_be(out, t3);
        out += 2;
        len += 2;
      }
    }
  }
  return len;
}

int32_t
blast_decode_file(char* in_filename,
                  int32_t type,
                  char* out_filename,
                  uint8_t* lut)
{
  uint8_t* in_buf = NULL;
  uint8_t* out_buf = NULL;
  int32_t in_len;
  int32_t write_len;
  int32_t out_len = 0;
  int32_t ret_val = 0;

  in_len = read_file(in_filename, &in_buf);
  if (in_len <= 0)
  {
    return 1;
  }

  // estimate worst case size
  out_buf = malloc(100 * in_len);
  if (out_buf == NULL)
  {
    ret_val = 2;
    goto free_all;
  }

  switch (type)
  {
    // a0 - input buffer
    // a1 - input length
    // a2 - type (always unused)
    // a3 - output buffer
    // t4 - blocks 4 & 5 reference t4 which is set to FP
    case 0:
      out_len = decode_block0(in_buf, in_len, out_buf);
      break;
    case 1:
      out_len = decode_block1(in_buf, in_len, out_buf);
      break;
    case 2:
      out_len = decode_block2(in_buf, in_len, out_buf);
      break;
    case 3:
      out_len = decode_block3(in_buf, in_len, out_buf);
      break;
    // TODO: need to figure out where last param is set for decoders 4 and 5
    case 4:
      out_len = decode_block4(in_buf, in_len, out_buf, lut);
      break;
    case 5:
      out_len = decode_block5(in_buf, in_len, out_buf, lut);
      break;
    case 6:
      out_len = decode_block6(in_buf, in_len, out_buf);
      break;
    default:
      ERROR("Unknown Blast type %d\n", type);
      break;
  }

  write_len = write_file(out_filename, out_buf, out_len);
  if (write_len != out_len)
  {
    ret_val = 2;
  }

free_all:
  if (out_buf)
  {
    free(out_buf);
  }
  if (in_buf)
  {
    free(in_buf);
  }

  return ret_val;
}

typedef struct
{
  uint8_t* src;    // w0
  uint32_t length; // w4
  uint32_t type;   // w8
  uint32_t wC;
} block_t;

// 802A57DC (06101C)
// a0 is only real parameters in ROM
int32_t
decompress_block(block_t* block,
                 uint8_t** decompressed_bytes,
                 uint8_t* rom_bytes)
{
  *decompressed_bytes = malloc(100 * block->length);

  switch (block->type)
  {
    // a0 - input buffer
    // a1 - input length
    // a2 - type (always unused)
    // a3 - output buffer
    // t4 - blocks 4 & 5 reference t4 which is set to FP
    case 0:
      return decode_block0(block->src, block->length, *decompressed_bytes);
    case 1:
      return decode_block1(block->src, block->length, *decompressed_bytes);
    case 2:
      return decode_block2(block->src, block->length, *decompressed_bytes);
    case 3:
      return decode_block3(block->src, block->length, *decompressed_bytes);
    // TODO: need to figure out where last param is set for decoders 4 and 5
    case 4:
      return decode_block4(
        block->src, block->length, *decompressed_bytes, &rom_bytes[0x047480]);
    // case 5: v0 = decode_block5(src, len, *copy, &rom[0x0998E0]); break;
    case 5:
      return decode_block5(
        block->src, block->length, *decompressed_bytes, &rom_bytes[0x152970]);
    // case 5: v0 = decode_block5(src, len, *copy, &rom[0x1E2C00]); break;
    case 6:
      return decode_block6(block->src, block->length, *decompressed_bytes);
    default:
      printf("Need type %d\n", block->type);
      return -1;
  }
}

bool
get_type_params(uint16_t type,
                int32_t decompressed_size,
                int32_t* width,
                int32_t* height,
                int32_t* depth,
                char** format)
{
  switch (type)
  {
    case 1:
      // guess at dims
      switch (decompressed_size)
      {
        // clang-format off
        case 16:   *width = 4;  *height = 2;  break;
        case 512:  *width = 16; *height = 16; break;
        case 1*KB: *width = 16; *height = 32; break;
        case 2*KB: *width = 32; *height = 32; break;
        case 4*KB: *width = 64; *height = 32; break;
        case 8*KB: *width = 64; *height = 64; break;
        case 3200: *width = 40; *height = 40; break;
        default:   *width = 32; *height = decompressed_size / *width / 2; break;
          // clang-format on
      }
      *format = "rgba";
      *depth = 16;
      return true;
    case 2:
      // guess at dims
      switch (decompressed_size)
      {
        // clang-format off
        case 256:  *width = 8;  *height = 8;  break;
        case 512:  *width = 8;  *height = 16; break;
        case 1*KB: *width = 16; *height = 16; break;
        case 2*KB: *width = 16; *height = 32; break;
        case 4*KB: *width = 32; *height = 32; break;
        case 8*KB: *width = 64; *height = 32; break;
        default:   *width = 32; *height = decompressed_size / *width / 4; break;
          // clang-format on
      }
      *format = "rgba";
      *depth = 32;
      return true;
    case 3:
      // guess at dims
      switch (decompressed_size)
      {
        // clang-format off
        case 1*KB: *width = 32; *height = 32; break;
        case 2*KB: *width = 32; *height = 64; break;
        case 4*KB: *width = 64; *height = 64; break;
        default:   *width = 32; *height = decompressed_size / *width; break;
          // clang-format on
      }
      *format = "ia";
      *depth = 8;
      return true;
    case 4:
      // guess at dims
      switch (decompressed_size)
      {
        // clang-format off
        case 1*KB: *width = 16; *height = 32; break;
        case 2*KB: *width = 32; *height = 32; break;
        case 4*KB: *width = 32; *height = 64; break;
        case 8*KB: *width = 64; *height = 64; break;
        default:   *width = 32; *height = decompressed_size / *width / 2; break;
          // clang-format on
      }
      *format = "ia";
      *depth = 16;
      return true;
    case 5:
      // guess at dims
      switch (decompressed_size)
      {
        // clang-format off
        case 1*KB: *width = 16; *height = 16; break;
        case 2*KB: *width = 32; *height = 16; break;
        case 4*KB: *width = 32; *height = 32; break;
        case 8*KB: *width = 64; *height = 32; break;
        default:   *width = 32; *height = decompressed_size / *width / 4; break;
          // clang-format on
      }
      *format = "rgba";
      *depth = 32;
      return true;
    case 6:
      // guess at dims
      *depth = 8;
      *width = 16;
      *height = (decompressed_size * 8 / *depth) / *width;
      *format = "ia";
      return true;
    default:
      return false;
  }
}

static void
convert_to_png(char* fname, uint16_t len, uint16_t type)
{
  char pngname[512];
  int32_t height, width, depth;
  rgba* rimg;
  ia* img;
  generate_filename(fname, pngname, "png");
  switch (type)
  {
    case 0:
      // TODO: memcpy, no info
      break;
    case 1:
      // guess at dims
      switch (len)
      {
        // clang-format off
        case 16:   width = 4;  height = 2;  break;
        case 512:  width = 16; height = 16; break;
        case 1*KB: width = 16; height = 32; break;
        case 2*KB: width = 32; height = 32; break;
        case 4*KB: width = 64; height = 32; break;
        case 8*KB: width = 64; height = 64; break;
        default:   width = 32; height = len/width/2; break;
          // clang-format on
      }
      // RGBA16
      rimg = file2rgba(fname, 0, width, height, 16);
      if (rimg)
        rgba2png(rimg, width, height, pngname);
      break;
    case 2:
      // guess at dims
      switch (len)
      {
        // clang-format off
        case 1*KB: width = 16; height = 16; break;
        case 2*KB: width = 16; height = 32; break;
        case 4*KB: width = 32; height = 32; break;
        case 8*KB: width = 64; height = 32; break;
        default:   width = 32; height = len/width/4; break;
          // clang-format on
      }
      // RGBA32
      rimg = file2rgba(fname, 0, width, height, 32);
      if (rimg)
        rgba2png(rimg, width, height, pngname);
      break;
    case 3:
      // guess at dims
      switch (len)
      {
        // clang-format off
        case 1*KB: width = 32; height = 32; break;
        case 2*KB: width = 32; height = 64; break;
        case 4*KB: width = 64; height = 64; break;
        default:   width = 32; height = len/width; break;
          // clang-format on
      }
      // IA8
      img = file2ia(fname, 0, width, height, 8);
      if (img)
        ia2png(img, width, height, pngname);
      break;
    case 4:
      // guess at dims
      switch (len)
      {
        // clang-format off
        case 1*KB: width = 32; height = 16; break;
        case 2*KB: width = 32; height = 32; break;
        case 4*KB: width = 32; height = 64; break;
        case 8*KB: width = 64; height = 64; break;
        default:   width = 32; height = len/width/2; break;
          // clang-format on
      }
      // IA16
      img = file2ia(fname, 0, width, height, 16);
      if (img)
        ia2png(img, width, height, pngname);
      break;
    case 5:
      // guess at dims
      switch (len)
      {
        // clang-format off
        case 1*KB: width = 16; height = 16; break;
        case 2*KB: width = 32; height = 16; break;
        case 4*KB: width = 32; height = 32; break;
        case 8*KB: width = 64; height = 32; break;
        default:   width = 32; height = len/width/2; break;
          // clang-format on
      }
      // RGBA32
      rimg = file2rgba(fname, 0, width, height, 32);
      if (rimg)
        rgba2png(rimg, width, height, pngname);
      break;
    case 6:
      // guess at dims
      depth = 8;
      width = 16;
      height = (len * 8 / depth) / width;
      // IA8
      img = file2ia(fname, 0, width, height, depth);
      if (img)
        ia2png(img, width, height, pngname);
      break;
  }
}

void
decompress_rom(const char* rom_path, uint8_t* rom_bytes, size_t rom_size)
{
  char* rom_dir_path = strdup(rom_path);
  dirname(rom_dir_path);

  // loop through from 0x4CE0 to 0xCCE0
  for (uint32_t address = ROM_OFFSET; address < END_OFFSET; address += 8)
  {
    uint32_t start = read_u32_be(&rom_bytes[address]);
    uint16_t compressed_size = read_u16_be(&rom_bytes[address + 4]);
    uint16_t type = read_u16_be(&rom_bytes[address + 6]);
    assert(rom_size >= start);
    // TODO: there are large sections of len=0, possibly LUTs for 4 & 5?

    if (compressed_size == 0)
    {
      continue;
    }

    block_t block = {
      .src = &rom_bytes[start + ROM_OFFSET],
      .length = compressed_size,
      .type = type,
    };
    // printf("%X (%X) %X %d\n", start, start+ROM_OFFSET, len, type);
    uint8_t* decompressed_bytes;
    int32_t decompressed_size =
      decompress_block(&block, &decompressed_bytes, rom_bytes);

    char* format = NULL;
    int32_t width = 0, height = 0, depth = 0;
    bool type_valid = get_type_params(
      type, decompressed_size, &width, &height, &depth, &format);

    if (!type_valid)
    {
      ERROR("Error: %d x %d for %X at %X type %d\n",
            width,
            height,
            decompressed_size,
            start + ROM_OFFSET,
            type);
      continue;
    }

    char format_str[16];
    sprintf(format_str, "(%s%d)", format, depth);

    printf("[0x%06X, 0x%06X] blast%d %8s %2dx%2d %4d -> %4d bytes\n",
           start + ROM_OFFSET,
           start + ROM_OFFSET + compressed_size,
           type,
           format_str,
           width,
           height,
           compressed_size,
           decompressed_size);

    // Write compressed file
    char out_path_compressed[512];
    sprintf(out_path_compressed,
            "%s/%06X.blast%d",
            rom_dir_path,
            start + ROM_OFFSET,
            type);
    write_file(out_path_compressed, rom_bytes, compressed_size);

    // Write decompressed file
    char out_path_decompressed[512];
    sprintf(out_path_decompressed,
            "%s/%06X.%s%d",
            rom_dir_path,
            start + ROM_OFFSET,
            format,
            depth);
    write_file(out_path_decompressed, decompressed_bytes, decompressed_size);

    // attempt to convert to PNG
    convert_to_png(out_path_decompressed, decompressed_size, type);
  }
}

void
print_usage()
{
  printf("Usage:\n");
  printf("./blast <rom_path>\n");
  printf("./blast <file_path> <blast_compression_id>\n");
}

int
main(int argc, char* argv[])
{
  if (argc < 2 || argc > 3)
  {
    print_usage();
    return EXIT_FAILURE;
  }

  // read in Blast Corps ROM
  uint8_t* rom_bytes;
  size_t size = read_file(argv[1], &rom_bytes);

  if (argc == 2)
  {
    // decompress whole rom
    decompress_rom(argv[1], rom_bytes, size);
  }
  else if (argc == 3)
  {
    // convert single file
    convert_to_png(argv[1], size, atoi(argv[2]));
  }

  free(rom_bytes);

  return EXIT_SUCCESS;
}
