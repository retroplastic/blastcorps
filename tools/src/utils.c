#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utime.h>

#include "utils.h"

size_t
read_file(const char* file_name, unsigned char** data)
{
  FILE* in;
  unsigned char* in_buf = NULL;
  size_t file_size;
  size_t bytes_read;
  in = fopen(file_name, "rb");
  if (in == NULL)
  {
    return -1;
  }

  // allocate buffer to read from offset to end of file
  fseek(in, 0, SEEK_END);
  file_size = ftell(in);

  // sanity check
  if (file_size > 256 * MB)
  {
    return -2;
  }

  in_buf = malloc(file_size);
  fseek(in, 0, SEEK_SET);

  // read bytes
  bytes_read = fread(in_buf, 1, file_size, in);
  if (bytes_read != file_size)
  {
    return -3;
  }

  fclose(in);
  *data = in_buf;
  return bytes_read;
}

size_t
write_file(const char* file_name, unsigned char* data, size_t length)
{
  FILE* out;
  size_t bytes_written;
  // open output file
  out = fopen(file_name, "wb");
  if (out == NULL)
  {
    perror(file_name);
    return -1;
  }
  bytes_written = fwrite(data, 1, length, out);
  fclose(out);
  return bytes_written;
}

void
generate_filename(const char* in_name, char* out_name, char* extension)
{
  char tmp_name[FILENAME_MAX];
  int len;
  int i;
  strcpy(tmp_name, in_name);
  len = strlen(tmp_name);
  for (i = len - 1; i > 0; i--)
  {
    if (tmp_name[i] == '.')
    {
      break;
    }
  }
  if (i <= 0)
  {
    i = len;
  }
  tmp_name[i] = '\0';
  sprintf(out_name, "%s.%s", tmp_name, extension);
}
