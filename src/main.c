#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "io.h"
#include "genhead.h"

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(*arr))

static struct {
  const char * source;
  const char * dest;
  const char * name;
} config;

static void print_usage(int exit_code) {
  FILE * fp = exit_code == 0 ? stdout : stderr;
  printf("%i\n", exit_code);
  fprintf(fp, "Usage: genhead [--help] [--name] [SOURCE] [DEST]\n");
  exit(exit_code);
}

static bool parse_args(int argc, char * argv[]) {
  argv++; argc--;

  config.source = NULL;
  config.dest = NULL;

  const char ** required_args[] = {
    &config.source,
    &config.dest
  };

  int i;
  size_t required_i;
  for (i = 0, required_i = 0; i < argc; ++i) {
    char * arg = argv[i];
    if (strncmp(arg, "--", 2) == 0) {
      arg += 2;
      if (strncmp(arg, "help", 4) == 0) {
        print_usage(0);
      } else if (strncmp(arg, "name", 4) == 0) {
        arg += 4;
        if (*arg == '=') {
          config.name = arg + 1;
        } else {
          return false;
        }
      } else {
        return false;
      }
    } else if (required_i < ARRAY_SIZE(required_args)) {
      *required_args[required_i++] = arg;
    } else {
      return false;
    }
  }

  if (config.name == NULL) {
    config.name = config.source;
  }

  return true;
}

static FILE * fopen_source() {
  if (config.source == NULL) {
    return stdin;
  }

  FILE * fp = fopen(config.source, "r");
  if (fp == NULL) {
    return NULL;
  }

  return fp;
}

static FILE * fopen_dest() {
  if (config.dest == NULL) {
    return stdout;
  }

  FILE * fp = fopen(config.dest, "w");
  if (fp == NULL) {
    return NULL;
  }

  return fp;
}

int main(int argc, char * argv[]) {
  if (!parse_args(argc, argv)) {
    print_usage(1);
  }

  FILE * source;
  if ((source = fopen_source()) == NULL) {
    fprintf(stderr, "%s\n", "Failed to open source file");
  } else {
    FILE * dest;
    if ((dest = fopen_dest()) == NULL) {
      fprintf(stderr, "%s\n", "Failed to open dest file");
    } else {
      genhead_generate(source, dest, config.name);
    }
    fclose(dest);
  }
  fclose(source);

  return 0;
}
