#pragma once

#include <stdio.h>

// defines

#define KB 1024
#define MB (1024 * KB)

// number of elements in statically declared array
#define DIM(S_ARR_) (sizeof(S_ARR_) / sizeof(S_ARR_[0]))

// read/write u32/16 big/little endian
#define read_u32_be(buf)                                                       \
  (unsigned int)(((buf)[0] << 24) + ((buf)[1] << 16) + ((buf)[2] << 8) +       \
                 ((buf)[3]))
#define read_u32_le(buf)                                                       \
  (unsigned int)(((buf)[1] << 24) + ((buf)[0] << 16) + ((buf)[3] << 8) +       \
                 ((buf)[2]))
#define write_u32_be(buf, val)                                                 \
  do                                                                           \
  {                                                                            \
    (buf)[0] = ((val) >> 24) & 0xFF;                                           \
    (buf)[1] = ((val) >> 16) & 0xFF;                                           \
    (buf)[2] = ((val) >> 8) & 0xFF;                                            \
    (buf)[3] = (val)&0xFF;                                                     \
  } while (0)
#define read_u16_be(buf) (((buf)[0] << 8) + ((buf)[1]))
#define write_u16_be(buf, val)                                                 \
  do                                                                           \
  {                                                                            \
    (buf)[0] = ((val) >> 8) & 0xFF;                                            \
    (buf)[1] = ((val)) & 0xFF;                                                 \
  } while (0)

#define ERROR(...) fprintf(stderr, __VA_ARGS__)

// functions

// read entire contents of file into buffer
// returns file size or negative on error
size_t
read_file(const char* file_name, unsigned char** data);

// write buffer to file
// returns number of bytes written out or -1 on failure
size_t
write_file(const char* file_name, unsigned char* data, size_t length);

// generate an output file name from input name by replacing file extension
// in_name: input file name
// out_name: buffer to write output name in
// extension: new file extension to use
void
generate_filename(const char* in_name, char* out_name, char* extension);
