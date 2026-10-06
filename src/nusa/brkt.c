// Copyright 2026 Pengembang Bahasa Pemrograman Nusa
// SPDX-License-Identifier: Apache-2.0

#include "nusa/brkt.h"
#include "nusa/target.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *register_argumen(int i) {
  const Target *t = get_target();
  if (i < t->banyak_reg) {
    return t->reg[i];
  }
  return "stack";
}

static void cetak_simbol(FILE *out, const InstruksiRA *cur) {
  const Target *t = get_target();

  if (strcmp(t->so, "windows") == 0 && !t->is_64) {
    fprintf(out, "_%s", cur->nama);
  } else {
    fprintf(out, "%s", cur->nama);
  }

  if (cur->eks)
    return;
  if (cur->pub && strcmp(t->so, "windows") == 0)
    fprintf(out, "_pub");
}

static void cetak_sisip_stack(FILE *out) {
  const Target *t = get_target();
  if (t->is_64)
    fprintf(out, "    subq $8, %%rsp\n");
}

static void cetak_pilha_stack(FILE *out) {
  const Target *t = get_target();
  if (t->is_64)
    fprintf(out, "    addq $8, %%rsp\n");
}

static int indeks_string = 0;

static void label_string(char *keluaran, size_t n, int id) {
  snprintf(keluaran, n, ".Lunt_%d", id);
}

static void cetak_string_rodata(FILE *out, const char *nilai) {
  size_t pjg = strlen(nilai);
  const char *isi = nilai;
  if (pjg >= 2 && isi[0] == '\'' && isi[pjg - 1] == '\'')
    isi += 1, pjg -= 2;

  fprintf(out, "    .ascii \"");
  for (size_t k = 0; k < pjg; k++) {
    if (isi[k] == '\\' && k + 1 < pjg && isi[k + 1] == 'n') {
      fprintf(out, "\\n");
      k++;
    } else if (isi[k] == '"' || isi[k] == '\\') {
      fprintf(out, "\\%c", isi[k]);
    } else {
      fputc(isi[k], out);
    }
  }
  fprintf(out, "\"\n");
  fprintf(out, "    .byte 0\n");
  fprintf(out, "    .p2align 3\n");
}

static void cetak_rodata(FILE *out, const InstruksiRA *daftar) {
  for (const InstruksiRA *cur = daftar; cur; cur = cur->next) {
    if (cur->badan)
      cetak_rodata(out, cur->badan);
    if (cur->tipe != RA_PANGGIL)
      continue;

    for (int i = 0; i < cur->jumlah; i++) {
      if (cur->tipe_nilai[i] != RA_UNTAIAN)
        continue;

      char label[128];
      label_string(label, sizeof(label), indeks_string++);

      fprintf(out, "%s:\n", label);
      cetak_string_rodata(out, cur->nilai[i]);
    }
  }
}

static void cetak_instruksi(FILE *out, const InstruksiRA *ins,
                            const InstruksiRA *fungsi) {
  for (const InstruksiRA *cur = ins; cur; cur = cur->next) {
    if (cur->tipe == RA_FUNGSI) {
      if (!cur->badan) {
        fprintf(out, ".extern ");
        cetak_simbol(out, cur);
        fprintf(out, "\n");
        continue;
      }

      if (cur->pub || !cur->modul) {
        fprintf(out, ".globl ");
        cetak_simbol(out, cur);
        fprintf(out, "\n");
      }

      cetak_simbol(out, cur);
      fprintf(out, ":\n");

      bool ada_panggil = false;
      for (const InstruksiRA *b = cur->badan; b && !ada_panggil; b = b->next)
        ada_panggil = b->tipe == RA_PANGGIL;

      if (ada_panggil)
        cetak_sisip_stack(out);

      if (cur->badan)
        cetak_instruksi(out, cur->badan, cur);

      if (ada_panggil)
        cetak_pilha_stack(out);

      fprintf(out, "    ret\n");
    } else if (cur->tipe == RA_PANGGIL) {
      const Target *t = get_target();
      if (!t->is_64) {
        for (int i = cur->jumlah - 1; i >= 0; i--) {
          if (cur->tipe_nilai[i] == RA_UNTAIAN) {
            char label[128];
            label_string(label, sizeof(label), indeks_string++);
            fprintf(out, "    pushl $%s\n", label);
          } else {
            bool is_param = false;
            if (fungsi) {
              for (int k = 0; k < fungsi->jumlah; k++) {
                if (strcmp(cur->nilai[i], fungsi->nilai[k]) == 0) {
                  fprintf(out, "    movl %d(%%ebp), %%eax\n", 8 + k * 4);
                  fprintf(out, "    pushl %%eax\n");
                  is_param = true;
                  break;
                }
              }
            }
            if (!is_param) {
              fprintf(out, "    pushl $%s\n", cur->nilai[i]);
            }
          }
        }

        fprintf(out, "    call ");
        cetak_simbol(out, cur);
        fprintf(out, "\n");

        if (cur->jumlah > 0) {
          fprintf(out, "    addl $%d, %%esp\n", cur->jumlah * 4);
        }
      } else {
        for (int i = 0; i < cur->jumlah; i++) {
          if (cur->tipe_nilai[i] == RA_UNTAIAN) {
            char label[128];
            label_string(label, sizeof(label), indeks_string++);
            fprintf(out, "    lea %s(%%rip), %s\n", label, register_argumen(i));
          } else if (i < t->banyak_reg) {
            bool is_param = false;
            if (fungsi) {
              for (int k = 0; k < fungsi->jumlah; k++) {
                if (strcmp(cur->nilai[i], fungsi->nilai[k]) == 0) {
                  const char *src = register_argumen(k);
                  const char *dst = register_argumen(i);
                  if (strcmp(src, dst) != 0)
                    fprintf(out, "    mov %s, %s\n", src, dst);
                  is_param = true;
                  break;
                }
              }
            }
            if (!is_param) {
              fprintf(out, "    mov $%s, %s\n", cur->nilai[i],
                      register_argumen(i));
            }
          } else {
            fprintf(out, "    ; arg %d lewat stack: %s\n", i, cur->nilai[i]);
          }
        }

        fprintf(out, "    call ");
        cetak_simbol(out, cur);
        fprintf(out, "\n");
      }
    }
  }
}

void bangkitkan_brkt(const InstruksiRA *daftar, const char *output_file) {
  if (!output_file) {
    printf("\nBahasa Rakitan (BRKT):\n");
    printf("; Target: %s %s\n", get_target()->so, get_target()->arsitektur);
  }

  const char *daftar_modul[256];
  int jumlah_modul = 0;

  for (const InstruksiRA *cur = daftar; cur; cur = cur->next) {
    const char *m = cur->modul ? cur->modul : "<program>";
    bool ada = false;
    for (int i = 0; i < jumlah_modul; i++) {
      if (strcmp(daftar_modul[i], m) == 0) {
        ada = true;
        break;
      }
    }
    if (!ada && jumlah_modul < 256) {
      daftar_modul[jumlah_modul++] = m;
    }
  }

  for (int m_idx = 0; m_idx < jumlah_modul; m_idx++) {
    const char *m = daftar_modul[m_idx];
    if (!output_file) {
      printf("\nBerkas %s:\n", m);
    }

    FILE *out = NULL;
    char *jalur_modul = NULL;

    if (output_file) {
      if (strcmp(m, "<program>") == 0) {
        jalur_modul = strdup(output_file);
      } else {
        size_t n = strlen(m) + 3;
        jalur_modul = malloc(n);
        snprintf(jalur_modul, n, "%s.s", m);
      }

      out = fopen(jalur_modul, "w");
      if (!out) {
        fprintf(stderr, "BRKT: gagal membuka berkas output %s\n", jalur_modul);
        free(jalur_modul);
        return;
      }
    } else {
      out = stdout;
    }

    indeks_string = 0;
    fprintf(out, ".section .rodata\n");
    for (const InstruksiRA *cur = daftar; cur; cur = cur->next) {
      const char *cm = cur->modul ? cur->modul : "<program>";
      if (strcmp(cm, m) != 0)
        continue;

      if (cur->badan)
        cetak_rodata(out, cur->badan);
      if (cur->tipe != RA_PANGGIL)
        continue;

      for (int i = 0; i < cur->jumlah; i++) {
        if (cur->tipe_nilai[i] != RA_UNTAIAN)
          continue;

        char label[128];
        label_string(label, sizeof(label), indeks_string++);

        fprintf(out, "%s:\n", label);
        cetak_string_rodata(out, cur->nilai[i]);
      }
    }

    indeks_string = 0;
    fprintf(out, ".text\n");
    for (const InstruksiRA *cur = daftar; cur; cur = cur->next) {
      const char *cm = cur->modul ? cur->modul : "<program>";
      if (strcmp(cm, m) != 0)
        continue;

      InstruksiRA single = *cur;
      single.next = NULL;
      cetak_instruksi(out, &single, NULL);
    }

    if (output_file) {
      fclose(out);
      free(jalur_modul);
      continue;
    }
  }

  if (!output_file) {
    printf("; Selesai Code Gen\n");
  }
}