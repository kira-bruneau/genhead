#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "genhead.h"

////////////////////////////////////////////////////////////////////////////////

typedef struct {
  size_t start;
  size_t end;
} Range;

ptrdiff_t range_diff(Range range) {
  return range.end - range.start;
}

////////////////////////////////////////////////////////////////////////////////

void genhead_init(GenHead * genhead, const char * filename, const char * data, size_t size) {
  genhead->filename = filename;
  genhead->data = data;
  genhead->size = size;
  genhead->index = 0;
}

static char * genhead_macro_name(GenHead * genhead) {
  char * extension = strrchr(genhead->filename, '.');

  size_t name_len;
  if (extension != NULL) {
    name_len = extension - genhead->filename;
  } else {
    name_len = strlen(genhead->filename);
  }

  char * macro_name = malloc(name_len + 3);
  if (macro_name == NULL) {
    return NULL;
  }

  size_t i;
  for (i = 0; i < name_len; ++i) {
    macro_name[i] = toupper(genhead->filename[i]);
  }

  macro_name[i++] = '_';
  macro_name[i++] = 'H';
  macro_name[i++] = '\0';
  return macro_name;
}

static char genhead_get(GenHead * genhead) {
  return genhead->data[genhead->index];
}

static void genhead_next(GenHead * genhead) {
  char c = genhead_get(genhead);
  if (c != '\0') {
    genhead->index++;
  }
}

static Range genhead_space(GenHead * genhead) {
  Range range;
  range.start = genhead->index;

  while (isspace(genhead_get(genhead))) {
    genhead_next(genhead);
  }

  range.end = genhead->index;
  return range;
}

static Range genhead_body(GenHead * genhead) {
  Range range;
  range.start = genhead->index;

  if (genhead_get(genhead) == '{') {
    do {
      genhead_next(genhead);
      genhead_body(genhead);
    } while (genhead_get(genhead) != '}');

    genhead_next(genhead);
  }

  range.end = genhead->index;
  return range;
}

static Range genhead_args(GenHead * genhead) {
  Range range;
  range.start = genhead->index;

  if (genhead_get(genhead) == '(') {
    do {
      genhead_next(genhead);
      genhead_args(genhead);
    } while (genhead_get(genhead) != ')');

    genhead_next(genhead);
  }

  range.end = genhead->index;
  return range;
}

static int symbolic_start(int c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static int symbolic(int c) {
  return symbolic_start(c) || (c >= '0' && c <= '9');
}

static Range genhead_symbol(GenHead * genhead) {
  Range range;
  range.start = genhead->index;

  if (symbolic_start(genhead_get(genhead))) {
    do {
      genhead_next(genhead);
    } while (symbolic(genhead_get(genhead)));
  }

  range.end = genhead->index;
  return range;
}

static Range genhead_type(GenHead * genhead) {
  Range range;
  range.start = range.end = genhead->index;

  if (range_diff(genhead_symbol(genhead)) != 0) {
    range.end = genhead->index;

    genhead_space(genhead);
    if (genhead_get(genhead) == '*') {
      genhead_next(genhead);
      range.end = genhead->index;
    }
  }

  genhead->index = range.end;
  return range;
}

static Range genhead_head(GenHead * genhead) {
  Range range;
  range.start = range.end = genhead->index;

  while (range_diff(genhead_type(genhead)) != 0) {
    range.end = genhead->index;
    genhead_space(genhead);
  }

  genhead->index = range.end;
  return range;
}

static Range genhead_signature(GenHead * genhead) {
  Range range;
  range.start = range.end = genhead->index;

  if (range_diff(genhead_head(genhead)) == 0) {
    return range;
  }

  genhead_space(genhead);

  if (range_diff(genhead_args(genhead)) == 0) {
    return range;
  }

  range.end = genhead->index;
  genhead_space(genhead);

  genhead_body(genhead);
  return range;
}

void genhead_generate(GenHead * genhead) {
  char * macro_name = genhead_macro_name(genhead);
  printf("#ifndef %s\n#define %s\n\n", macro_name, macro_name);
  free(macro_name);

  while (genhead_get(genhead)) {
    genhead_space(genhead);

    Range test_range, signature;
    test_range.start = genhead->index;
    signature = genhead_signature(genhead);
    test_range.end = genhead->index;

    if (range_diff(signature) != 0) {
      printf("%.*s;\n", (int)range_diff(signature), &genhead->data[signature.start]);
    } else if (range_diff(test_range) == 0) {
      genhead_next(genhead);
    }
  }

  printf("\n#endif\n");
}
