#include <assert.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blast.h"
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

// 802A57DC (06101C)
// a0 is only real parameters in ROM
int32_t
decompress_block(uint8_t* src,
                 uint32_t size,
                 blast_t type,
                 uint8_t* result_bytes,
                 uint8_t* lut)
{
  switch (type)
  {
    case BLAST0:
      return decode_block0(src, size, result_bytes);
    case BLAST1_RGBA16:
      return decode_block1(src, size, result_bytes);
    case BLAST2_RGBA32:
      return decode_block2(src, size, result_bytes);
    case BLAST3_IA8:
      return decode_block3(src, size, result_bytes);
    case BLAST4_IA16:
      return decode_block4(src, size, result_bytes, lut);
    case BLAST5_RGBA32:
      return decode_block5(src, size, result_bytes, lut);
    case BLAST6_IA8:
      return decode_block6(src, size, result_bytes);
    default:
      assert(false);
      return -1;
  }
}

const char*
get_type_format_name(blast_t type)
{
  switch (type)
  {
    case BLAST1_RGBA16:
    case BLAST2_RGBA32:
    case BLAST5_RGBA32:
      return "rgba";
    case BLAST3_IA8:
    case BLAST4_IA16:
    case BLAST6_IA8:
      return "ia";
    default:
      assert(false);
      return "";
  }
}

int32_t
get_type_depth(blast_t type)
{
  switch (type)
  {
    case BLAST3_IA8:
    case BLAST6_IA8:
      return 8;
    case BLAST1_RGBA16:
    case BLAST4_IA16:
      return 16;
    case BLAST2_RGBA32:
    case BLAST5_RGBA32:
      return 32;
    default:
      assert(false);
      return 0;
  }
}

// This cannot be reliably determined by the data and needs to be
// tracked manually in the splat yaml.
res_t
guess_resolution(blast_t type, int32_t size)
{
  switch (type)
  {
    case BLAST1_RGBA16:
      switch (size)
      {
        // clang-format off
        case 16:   return (res_t){ 4, 2 };
        case 512:  return (res_t){ 16, 16 };
        case 1*KB: return (res_t){ 16, 32 };
        case 2*KB: return (res_t){ 32, 32 };
        case 4*KB: return (res_t){ 64, 32 };
        case 8*KB: return (res_t){ 64, 64 };
        case 3200: return (res_t){ 40, 40 };
        default:   return (res_t){ 32, size / (32 * 2) };
          // clang-format on
      }
    case BLAST2_RGBA32:
      switch (size)
      {
        // clang-format off
        case 256:  return (res_t){ 8, 8 };
        case 512:  return (res_t){ 8, 16 };
        case 1*KB: return (res_t){ 16, 16 };
        case 2*KB: return (res_t){ 16, 32 };
        case 4*KB: return (res_t){ 32, 32 };
        case 8*KB: return (res_t){ 64, 32 };
        default:   return (res_t){ 32, size / (32 * 4) };
          // clang-format on
      }
    case BLAST3_IA8:
      switch (size)
      {
        // clang-format off
        case 1*KB: return (res_t){ 32, 32 };
        case 2*KB: return (res_t){ 32, 64 };
        case 4*KB: return (res_t){ 64, 64 };
        default:   return (res_t){ 32, size / 32 };
          // clang-format on
      }
    case BLAST4_IA16:
      switch (size)
      {
        // clang-format off
        case 1*KB: return (res_t){ 16, 32 };
        case 2*KB: return (res_t){ 32, 32 };
        case 4*KB: return (res_t){ 32, 64 };
        case 8*KB: return (res_t){ 64, 64 };
        default:   return (res_t){ 32, size / (32 * 2) };
          // clang-format on
      }
    case BLAST5_RGBA32:
      switch (size)
      {
        // clang-format off
        case 1*KB: return (res_t){ 16, 16 };
        case 2*KB: return (res_t){ 32, 16 };
        case 4*KB: return (res_t){ 32, 32 };
        case 8*KB: return (res_t){ 64, 32 };
        default:   return (res_t){ 32, size / (32 * 4) };
          // clang-format on
      }
    case BLAST6_IA8:
      return (res_t){ 16, size / 16 };
    default:
      assert(false);
  }
}

bool
convert_to_png(char* fname, uint16_t len, blast_t type)
{
  if (type == BLAST0)
  {
    return false;
  }

  res_t res = guess_resolution(type, len);

  int32_t depth = get_type_depth(type);

  char pngname[512];
  generate_filename(fname, pngname, "png");
  switch (type)
  {
    case BLAST1_RGBA16:
    case BLAST2_RGBA32:
    case BLAST5_RGBA32:
    {
      rgba* rimg = file2rgba(fname, 0, res.w, res.h, depth);
      if (!rimg)
      {
        return false;
      }
      rgba2png(rimg, res.w, res.h, pngname);
      return true;
    }
    case BLAST3_IA8:
    case BLAST4_IA16:
    case BLAST6_IA8:
    {
      ia* img = file2ia(fname, 0, res.w, res.h, depth);
      if (!img)
      {
        return false;
      }
      ia2png(img, res.w, res.h, pngname);
      return true;
    }
    default:
      return false;
  }
}
