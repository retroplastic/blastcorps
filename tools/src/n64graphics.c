#include <stdio.h>
#include <stdlib.h>

#define STBI_NO_LINEAR
#define STBI_NO_HDR
#define STBI_NO_TGA
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "n64graphics.h"
#include "utils.h"

// upscale 5-bit integer to 8-bit
#define SCALE_5_8(VAL_) (((VAL_)*0xFF) / 0x1F)
#define SCALE_8_5(VAL_) ((((VAL_) + 4) * 0x1F) / 0xFF)
#define SCALE_4_8(VAL_) ((VAL_)*0x11)
#define SCALE_8_4(VAL_) ((VAL_) / 0x11)
#define SCALE_3_8(VAL_) ((VAL_)*0x24)
#define SCALE_8_3(VAL_) ((VAL_) / 0x24)

typedef enum
{
  IMG_FORMAT_RGBA,
  IMG_FORMAT_IA,
  IMG_FORMAT_I,
  IMG_FORMAT_CI,
  IMG_FORMAT_SKYBOX,
} img_format;

rgba*
raw2rgba(char* raw, int width, int height, int depth)
{
  rgba* img = NULL;
  unsigned img_size;
  int i;

  img_size = width * height * sizeof(*img);
  img = malloc(img_size);
  if (!img)
  {
    ERROR("Error allocating %u bytes\n", img_size);
    return NULL;
  }

  if (depth == 16)
  {
    for (i = 0; i < width * height; i++)
    {
      img[i].red = SCALE_5_8((raw[i * 2] & 0xF8) >> 3);
      img[i].green =
        SCALE_5_8(((raw[i * 2] & 0x07) << 2) | ((raw[i * 2 + 1] & 0xC0) >> 6));
      img[i].blue = SCALE_5_8((raw[i * 2 + 1] & 0x3E) >> 1);
      img[i].alpha = (raw[i * 2 + 1] & 0x01) ? 0xFF : 0x00;
    }
  }
  else if (depth == 32)
  {
    for (i = 0; i < width * height; i++)
    {
      img[i].red = raw[i * 4];
      img[i].green = raw[i * 4 + 1];
      img[i].blue = raw[i * 4 + 2];
      img[i].alpha = raw[i * 4 + 3];
    }
  }

  return img;
}

ia*
raw2ia(char* raw, int width, int height, int depth)
{
  ia* img = NULL;
  unsigned img_size;
  int i;

  img_size = width * height * sizeof(*img);
  img = malloc(img_size);
  if (!img)
  {
    ERROR("Error allocating %u bytes\n", img_size);
    return NULL;
  }

  switch (depth)
  {
    case 16:
      for (i = 0; i < width * height; i++)
      {
        img[i].intensity = raw[i * 2];
        img[i].alpha = raw[i * 2 + 1];
      }
      break;
    case 8:
      for (i = 0; i < width * height; i++)
      {
        img[i].intensity = SCALE_4_8((raw[i] & 0xF0) >> 4);
        img[i].alpha = SCALE_4_8(raw[i] & 0x0F);
      }
      break;
    case 4:
      for (i = 0; i < width * height; i++)
      {
        unsigned char bits;
        bits = raw[i / 2];
        if (i % 2)
        {
          bits &= 0xF;
        }
        else
        {
          bits >>= 4;
        }
        img[i].intensity = SCALE_3_8((bits >> 1) & 0x07);
        img[i].alpha = (bits & 0x01) ? 0xFF : 0x00;
      }
      break;
    case 1:
      for (i = 0; i < width * height; i++)
      {
        unsigned char bits;
        unsigned char mask;
        bits = raw[i / 8];
        mask = 1 << (7 - (i % 8)); // MSb->LSb
        bits = (bits & mask) ? 0xFF : 0x00;
        img[i].intensity = bits;
        img[i].alpha = bits;
      }
      break;
    default:
      ERROR("Error invalid depth %d\n", depth);
      break;
  }

  return img;
}

rgba*
file2rgba(char* filename, int offset, int width, int height, int depth)
{
  FILE* fp;
  rgba* img = NULL;
  char* raw;
  unsigned size;

  fp = fopen(filename, "rb");
  if (!fp)
  {
    return NULL;
  }

  if (fseek(fp, offset, SEEK_SET))
  {
    ERROR("Error seeking to 0x%X in file '%s'\n", offset, filename);
    goto file2rgba_close;
  }

  size = width * height * depth / 8;
  raw = malloc(size);
  if (!raw)
  {
    ERROR("Error allocating %u bytes\n", size);
    goto file2rgba_close;
  }
  if (fread(raw, 1, size, fp) != size)
  {
    ERROR(
      "Error reading 0x%X bytes at 0x%X from '%s'\n", size, offset, filename);
    goto file2rgba_free;
  }

  img = raw2rgba(raw, width, height, depth);

file2rgba_free:
  free(raw);
file2rgba_close:
  fclose(fp);

  return img;
}

ia*
file2ia(char* filename, int offset, int width, int height, int depth)
{
  FILE* fp;
  ia* img = NULL;
  char* raw;
  unsigned size;
  unsigned img_size;

  fp = fopen(filename, "rb");
  if (!fp)
  {
    return NULL;
  }

  if (fseek(fp, offset, SEEK_SET))
  {
    ERROR("Error seeking to 0x%X in file '%s'\n", offset, filename);
    goto file2ia_close;
  }

  size = width * height * depth / 8;
  raw = malloc(size);
  if (!raw)
  {
    ERROR("Error allocating %u bytes\n", size);
    goto file2ia_close;
  }
  if (fread(raw, 1, size, fp) != size)
  {
    ERROR(
      "Error reading 0x%X bytes at 0x%X from '%s'\n", size, offset, filename);
    goto file2ia_free;
  }

  img_size = width * height * sizeof(*img);
  img = malloc(img_size);
  if (!img)
  {
    ERROR("Error allocating %u bytes\n", img_size);
    goto file2ia_free;
  }

  img = raw2ia(raw, width, height, depth);

file2ia_free:
  free(raw);
file2ia_close:
  fclose(fp);

  return img;
}

int
rgba2png(rgba* img, int width, int height, char* pngname)
{
  int ret = 0;
  unsigned char* data = malloc(4 * width * height);

  if (data)
  {
    for (int j = 0; j < height; j++)
    {
      for (int i = 0; i < width; i++)
      {
        int idx = j * width + i;
        data[4 * idx] = img[idx].red;
        data[4 * idx + 1] = img[idx].green;
        data[4 * idx + 2] = img[idx].blue;
        data[4 * idx + 3] = img[idx].alpha;
      }
    }

    // write the image data
    ret = stbi_write_png(pngname, width, height, 4, data, 0);

    free(data);
  }

  return ret;
}

int
ia2png(ia* img, int width, int height, char* pngname)
{
  int ret = 0;
  unsigned char* data = malloc(2 * width * height);

  if (data)
  {
    for (int j = 0; j < height; j++)
    {
      for (int i = 0; i < width; i++)
      {
        int idx = j * width + i;
        data[2 * idx] = img[idx].intensity;
        data[2 * idx + 1] = img[idx].alpha;
      }
    }

    // write the image data
    ret = stbi_write_png(pngname, width, height, 2, data, 0);

    free(data);
  }

  return ret;
}
