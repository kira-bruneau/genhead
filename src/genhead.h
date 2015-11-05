#ifndef GENHEAD_H
#define GENHEAD_H

#include <stddef.h>

typedef struct {
  const char * filename;
  const char * data;
  size_t size;
  size_t index;
} GenHead;

void genhead_init(GenHead * genhead, const char * filename, const char * data, size_t size);
void genhead_generate(GenHead * genhead);

#endif
