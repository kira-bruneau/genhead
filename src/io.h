#ifndef IO_H
#define IO_H

#include <stdio.h>

size_t fsize(FILE * fp);
char * fread_static(FILE * fp, size_t size);
char * fread_dynamic(FILE * fp);

#endif
