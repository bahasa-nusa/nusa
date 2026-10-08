#include "nusa/target.h"
#include <string.h>

static Target current_target = {NULL, NULL, 0, {NULL, NULL, NULL, NULL}, 0};

const Target *set_target(const char *so, const char *arsitektur) {
  current_target.so = so;
  current_target.arsitektur = arsitektur;

  if (strcmp(arsitektur, "intel64") == 0) {
    current_target.is_64 = 1;
    if (strcmp(so, "wins") == 0) {
      current_target.reg[0] = "%rcx";
      current_target.reg[1] = "%rdx";
      current_target.reg[2] = "%r8";
      current_target.reg[3] = "%r9";
    } else {
      current_target.reg[0] = "%rdi";
      current_target.reg[1] = "%rsi";
      current_target.reg[2] = "%rdx";
      current_target.reg[3] = "%rcx";
    }
    current_target.banyak_reg = 4;
  } else {
    current_target.is_64 = 0;
    current_target.banyak_reg = 0;
  }

  return &current_target;
}

const Target *get_target(void) { return &current_target; }
