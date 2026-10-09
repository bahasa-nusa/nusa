// Copyright 2026 Pengembang Bahasa Pemrograman Nusa
// SPDX-License-Identifier: Apache-2.0

#ifndef NUSA_TARGET_H
#define NUSA_TARGET_H

typedef struct {
  const char *target;
  int is_64;
  const char *reg[4];
  int banyak_reg;
} Target;

const Target *set_target(const char *target);
const Target *get_target(void);

#endif
