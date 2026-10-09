#include "nusa/target.h"
#include <stdio.h>
#include <string.h>
#include <llvm-c/Core.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>

static Target current_target = {NULL, 0, {NULL, NULL, NULL, NULL}, 0};

static void init_targets(void) {
  LLVMInitializeAllTargetInfos();
  LLVMInitializeAllTargets();
  LLVMInitializeAllTargetMCs();
}

const Target *set_target(const char *target) {
  init_targets();
  LLVMTargetRef t = NULL;
  char *err = NULL;
  if (LLVMGetTargetFromTriple(target, &t, &err)) {
    printf("Target tidak didukung LLVM: %s\n", target);
    if (err) {
      printf("%s\n", err);
      LLVMDisposeMessage(err);
    }
    return NULL;
  }

  current_target.target = target;
  current_target.is_64 = (strstr(target, "64") != NULL);

  if (strstr(target, "windows") || strstr(target, "win32")) {
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
  current_target.banyak_reg = current_target.is_64 ? 4 : 0;
  return &current_target;
}

const Target *get_target(void) { return &current_target; }
