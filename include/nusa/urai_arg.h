#ifndef NUSA_URAI_ARG_H
#define NUSA_URAI_ARG_H

#include <stdbool.h>

typedef struct {
  bool versi;
  bool info;
  const char *input_file;
} Arg;

Arg urai_arg(int jum_arg, char **isi_arg);

#endif
