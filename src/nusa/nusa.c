#include <stdio.h>

#include "nusa/urai_arg.h"

void cetak_info() {
    printf("Penggunaan: nusa [argumen] <berkas>\n\n");
    printf("Opsi:\n");
    printf("-v, --versi     Untuk melihat versi.\n");
    printf("-i, --info      Untuk melihat informasi penggunaan.\n");
}

int main(int argc, char** argv) {
    Arg arg = urai_arg(argc, argv);

    if (arg.versi) {
        printf("nusa v0.0.0\n");
        return 0;
    }

    if (arg.info) {
        cetak_info();
        return 0;
    }

    if (arg.input_file) {
        printf("Nama berkas: %s\n", arg.input_file);
    } else {
        printf("Argumen tidak valid.\n");
        cetak_info();
    }

    return 0;
}