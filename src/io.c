#include <stdlib.h>
#include <sys/stat.h>

#include "io.h"

size_t fsize(FILE * fp) {
  struct stat buf;
  if (fstat(fileno(fp), &buf) < 0) {
    return 0;
  }

  return buf.st_size;
}

char * fread_static(FILE * fp, size_t size) {
  char * data = malloc(size);
  if (data == NULL) {
    return NULL;
  }

  size_t num_read = fread(data, sizeof(char), size, fp);
  if (num_read != size) {
    free(data);
    return NULL;
  }

  return data;
}

char * fread_dynamic(FILE * fp) {
  size_t capacity = 256;
  char * data = malloc(capacity);
  if (data == NULL) {
    return NULL;
  }

  size_t size = 0;
  while ((size += fread(data + size, sizeof(char), capacity - size, fp)) == capacity) {
    char * new_data = realloc(data, capacity += capacity / 2);
    if (new_data == NULL) {
      free(data);
      return NULL;
    }

    data = new_data;
  }

  return data;
}
