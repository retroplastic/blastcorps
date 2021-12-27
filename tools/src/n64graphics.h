#pragma once

typedef struct _rgba
{
   unsigned char red;
   unsigned char green;
   unsigned char blue;
   unsigned char alpha;
} rgba;

typedef struct _ia
{
   unsigned char intensity;
   unsigned char alpha;
} ia;

// extract RGBA image from file at offset to RGBA data
rgba *file2rgba(char *filename, int offset, int width, int height, int depth);

// extract IA image from file at offset to RGBA data
ia *file2ia(char *filename, int offset, int width, int height, int depth);

// save RGBA data to PNG file
int rgba2png(rgba *img, int width, int height, char *pngname);

// save IA data to grayscale PNG file
int ia2png(ia *img, int width, int height, char *pngname);
