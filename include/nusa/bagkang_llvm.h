// Copyright 2026 Pengembang Bahasa Pemrograman Nusa
// SPDX-License-Identifier: Apache-2.0

#ifndef NUSA_BAGKANG_LLVM_H
#define NUSA_BAGKANG_LLVM_H

#include <stdbool.h>

void bagkang_llvm_daftarkan(void);

bool bagkang_llvm_target_tersedia(const char *target);

void bagkang_llvm_info_target(void);

#endif