#include <string.h>

#include "nusa/urai_arg.h"

Arg urai_arg(int argc, char** argv) {
    Arg args = {0};
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--versi") == 0) {
            args.versi = true;
        } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--info") == 0) {
            args.info = true;
        } else if (argv[i][0] != '-') {
            args.input_file = argv[i];
        }
    }
    return args;
}
