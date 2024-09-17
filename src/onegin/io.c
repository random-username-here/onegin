#include "onegin.h"
#include "istd/util/err.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

char* read_file(const char* filename) {
 
  FILE* f = fopen(filename, "r");
  if (!f)
    panic$("Failed to open given file\n"
          "  errno says   : %s\n"
          "  file name is : `%s`\n",
          strerror(errno), filename);

  struct stat info = { 0 };
  fstat(fileno(f), &info);
  size_t len = info.st_size;

  char* buf = calloc(len+1, sizeof(char));
  if (!buf)
    panic$("Failed to allocate buffer of length %zu for poem text\n", len+1);

  len = fread(buf, 1, len, f);
  buf[len] = 0;

  if (fclose(f))
    panic$("Failed to close input file for some reason");

  return buf;
}
