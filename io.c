#include <sys/stat.h>

#include "io.h"

size_t fsize(FILE * fp) {
  struct stat buf;
  if (fstat(fileno(fp), &buf) < 0) {
    return 0;
  }

  return buf.st_size;
}
