#include <stdlib.h>

#include "io.h"
#include "genhead.h"

int main(int argc, char * argv[]) {
  if (argc <= 1) {
    fprintf(stderr, "%s\n", "Input file not provided");
    return 1;
  }

  const char * filename = argv[1];
  FILE * fp = fopen(filename, "r");
  if (fp == NULL) {
    fprintf(stderr, "%s\n", "Failed to open file");
    return 1;
  }

  size_t size = fsize(fp);
  char * data = malloc(size);
  size_t num_read = fread(data, sizeof(char), size, fp);

  int exit_status = 0;
  if (num_read == size) {
    GenHead genhead;
    genhead_init(&genhead, filename, data, size);
    genhead_generate(&genhead);
  } else {
    fprintf(stderr, "%s\n", "Failed to read file");
    exit_status = 1;
  }

  free(data);
  fclose(fp);
  return exit_status;
}
