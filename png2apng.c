#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <subpng.h>

int main(int argc, char *argv[])
{
    FILE *fin, *fout;

    if (fopen(argv[1], "rb")) {
        fprintf(stderr, "File %s already exists.\n", argv[1]);
        exit(1);
    }
    if ((fout = fopen(argv[1], "wb")) == NULL) {
        fprintf(stderr, "Output file %s open fails.\n", argv[1]);
        exit(2);
    }

    fclose(fout);
    exit(0);
}
