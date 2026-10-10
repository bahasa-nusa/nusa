// Copyright 2026 Pengembang Bahasa Pemrograman Nusa
// SPDX-License-Identifier: Apache-2.0

#include "nusa/bagkang_llvm.h"
#include "nusa/jembat.h"
#include "nusa/ra.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <llvm-c/Core.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>

static int bagkang_llvm_jembat(const InstruksiRA *ra, const OpsiBackend *opsi);

static LLVMContextRef llvm_context = NULL;

static void init_llvm_targets(void) {
    static bool initialized = false;
    if (!initialized) {
        llvm_context = LLVMContextCreate();
        LLVMInitializeAllTargetInfos();
        LLVMInitializeAllTargets();
        LLVMInitializeAllTargetMCs();
        LLVMInitializeAllAsmParsers();
        LLVMInitializeAllAsmPrinters();
        initialized = true;
    }
}

void bagkang_llvm_daftarkan(void) {
    init_llvm_targets();
    jembat_daftarkan("llvm", bagkang_llvm_jembat);
}

bool bagkang_llvm_target_tersedia(const char *target) {
    if (!target) return false;

    init_llvm_targets();

    LLVMTargetRef t = NULL;
    char *err = NULL;
    bool success = !LLVMGetTargetFromTriple(target, &t, &err);

    if (err) {
        LLVMDisposeMessage(err);
    }

    return success;
}

void bagkang_llvm_info_target(void) {
    init_llvm_targets();

    printf("Target LLVM yang didukung:\n");

    LLVMTargetRef target_ref = LLVMGetFirstTarget();
    while (target_ref != NULL) {
        const char *name = LLVMGetTargetName(target_ref);
        const char *desc = LLVMGetTargetDescription(target_ref);
        printf("  %s: %s\n", name ? name : "(unknown)", desc ? desc : "(no description)");
        target_ref = LLVMGetNextTarget(target_ref);
    }
}

static LLVMTypeRef ra_tipe_ke_llvm_type(TipeNilaiRA tipe) {
    if (!llvm_context) init_llvm_targets();
    switch (tipe) {
        case RA_BILANGAN:
            return LLVMInt32TypeInContext(llvm_context);
        case RA_UNTAIAN:
            return LLVMPointerType(LLVMInt8TypeInContext(llvm_context), 0); // i8*
        case RA_TANPA_TIPE:
        default:
            return LLVMVoidTypeInContext(llvm_context);
    }
}

static char *lepas_untai(const char *s) {
    if (!s) return NULL;

    size_t len = strlen(s);
    if (len >= 2 && (s[0] == '"' || s[0] == '\'') && s[len - 1] == s[0]) {
        s++;
        len -= 2;
    }

    char *r = malloc(len + 1);
    if (!r) return NULL;

    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        if (s[i] == '\\' && i + 1 < len) {
            switch (s[i + 1]) {
                case 'n': r[j++] = '\n'; i++; break;
                case 't': r[j++] = '\t'; i++; break;
                case 'r': r[j++] = '\r'; i++; break;
                case '0': r[j++] = '\0'; i++; break;
                case '\\': r[j++] = '\\'; i++; break;
                case '"': r[j++] = '"'; i++; break;
                case '\'': r[j++] = '\''; i++; break;
                default: r[j++] = s[i]; break;
            }
        } else {
            r[j++] = s[i];
        }
    }
    r[j] = '\0';
    return r;
}

static LLVMValueRef buat_global_string(LLVMModuleRef module, const char *str) {
    if (!str) return NULL;

    size_t len = strlen(str);
    LLVMTypeRef arrayType = LLVMArrayType(LLVMInt8TypeInContext(llvm_context), len + 1);

    LLVMValueRef globalStr = LLVMAddGlobal(module, arrayType, ".str");
    LLVMSetLinkage(globalStr, LLVMPrivateLinkage);
    LLVMSetGlobalConstant(globalStr, true);
    LLVMSetInitializer(globalStr, LLVMConstStringInContext(llvm_context, str, (unsigned)len, true));

    return globalStr;
}

static int bagkang_llvm_jembat(const InstruksiRA *ra, const OpsiBackend *opsi) {
    if (!ra || !opsi) {
        return -1;
    }

    if (opsi->target && !bagkang_llvm_target_tersedia(opsi->target)) {
        fprintf(stderr, "Target tidak didukung LLVM: %s\n", opsi->target);
        return -1;
    }

    LLVMModuleRef module = LLVMModuleCreateWithNameInContext("nusa", llvm_context);
    LLVMBuilderRef builder = LLVMCreateBuilderInContext(llvm_context);

    if (opsi->target) {
        LLVMSetTarget(module, opsi->target);
    }

    for (const InstruksiRA *cur = ra; cur; cur = cur->next) {
        if (cur->tipe != RA_FUNGSI)
            continue;

        LLVMTypeRef ret = ra_tipe_ke_llvm_type(cur->tipe_kembali);
        LLVMTypeRef *params = NULL;
        int n = 0;

        if (cur->jumlah > 0 && cur->tipe_nilai) {
            params = malloc(sizeof(LLVMTypeRef) * cur->jumlah);
            for (int i = 0; i < cur->jumlah; i++) {
                params[n++] = ra_tipe_ke_llvm_type(cur->tipe_nilai[i]);
            }
        }

        LLVMTypeRef ft = LLVMFunctionType(ret, params, n, false);
        if (params) free(params);

        LLVMValueRef f = LLVMAddFunction(module, cur->nama, ft);
        if (cur->eks) {
            // deklarasi eksternal
            LLVMSetLinkage(f, LLVMExternalLinkage);
        }
    }

    for (const InstruksiRA *cur = ra; cur; cur = cur->next) {
        if (cur->tipe != RA_FUNGSI)
            continue;
        if (cur->eks || !cur->badan)
            continue;

        LLVMValueRef f = LLVMGetNamedFunction(module, cur->nama);
        if (!f)
            continue;

        LLVMBasicBlockRef entry = LLVMAppendBasicBlockInContext(llvm_context, f, "entry");
        LLVMPositionBuilderAtEnd(builder, entry);

        for (const InstruksiRA *instr = cur->badan; instr; instr = instr->next) {
            if (instr->tipe != RA_PANGGIL)
                continue;

            LLVMValueRef callee = LLVMGetNamedFunction(module, instr->nama);
            if (!callee) {
                LLVMTypeRef ft = LLVMFunctionType(LLVMVoidTypeInContext(llvm_context), NULL, 0, false);
                callee = LLVMAddFunction(module, instr->nama, ft);
            }

            LLVMValueRef args[16];
            int arg_count = 0;

            for (int i = 0; i < instr->jumlah && arg_count < 16; i++) {
                if (!instr->tipe_nilai || !instr->nilai) break;
                switch (instr->tipe_nilai[i]) {
                    case RA_BILANGAN: {
                        long nilai = strtol(instr->nilai[i], NULL, 10);
                        args[arg_count++] = LLVMConstInt(LLVMInt32TypeInContext(llvm_context), (unsigned long long)nilai, false);
                        break;
                    }
                    case RA_UNTAIAN: {
                        char *isi = lepas_untai(instr->nilai[i]);
                        if (isi) {
                            LLVMValueRef g = buat_global_string(module, isi);
                            free(isi);
                            if (g) {
                                args[arg_count++] = LLVMBuildPointerCast(builder, g, LLVMPointerType(LLVMInt8TypeInContext(llvm_context), 0), "strptr");
                            }
                        }
                        break;
                    }
                    case RA_TANPA_TIPE:
                    default:
                        break;
                }
            }

            LLVMBuildCall2(builder, LLVMGlobalGetValueType(callee), callee, args, arg_count, "");
        }

        if (cur->tipe_kembali == RA_TANPA_TIPE) {
            LLVMBuildRetVoid(builder);
        } else {
            LLVMBuildRet(builder, LLVMConstInt(ra_tipe_ke_llvm_type(cur->tipe_kembali), 0, false));
        }
    }

    if (opsi->bentuk && strcmp(opsi->bentuk, "ra") == 0) {
        char *ir = LLVMPrintModuleToString(module);
        if (opsi->berkas_keluar) {
            FILE *file = fopen(opsi->berkas_keluar, "w");
            if (file) {
                fprintf(file, "%s", ir);
                fclose(file);
            } else {
                fprintf(stderr, "Gagal menulis ke berkas: %s\n", opsi->berkas_keluar);
                LLVMDisposeMessage(ir);
                LLVMDisposeBuilder(builder);
                LLVMDisposeModule(module);
                return -1;
            }
        } else {
            printf("%s", ir);
        }
        LLVMDisposeMessage(ir);
    } else if (opsi->bentuk && strcmp(opsi->bentuk, "rkt") == 0) {
        fprintf(stderr, "Backend ASM belum tersedia\n");
        LLVMDisposeBuilder(builder);
        LLVMDisposeModule(module);
        return -1;
    }

    LLVMDisposeBuilder(builder);
    LLVMDisposeModule(module);
    return 0;
}