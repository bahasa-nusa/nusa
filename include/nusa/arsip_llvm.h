#ifndef NUSA_ARSIP_LLVM_H
#define NUSA_ARSIP_LLVM_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

int bagkang_llvm_buat_arsip(const char *berkas_keluar,
                            const char **daftar_berkas,
                            int jumlah_berkas);

#ifdef __cplusplus
}
#endif

#endif

