#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <subpng.h>

int main(int argc, char *argv[])
{
    FILE *fout;
    struct png_image *png_out, *png_in;
    int i;

    if (fopen(argv[1], "rb")) {
        fprintf(stderr, "File %s already exists.\n", argv[1]);
        exit(1);
    }

    if ((fout = fopen(argv[1], "wb")) == NULL) {
        fprintf(stderr, "Output file %s open fails.\n", argv[1]);
        exit(2);
    }

    if ((png_out = png_create()) == NULL) {
        exit(3);
    }

    for (i = 2 ; i < argc ; i++) {
        if ((png_in = png_load(argv[i])) == FALSE)
            exit(3);

        if (i == 2) {
            png_chunk_add_from_png(png_out, png_in, 0);
        }
    }

    printf("Finished\n");
    fclose(fout);
    exit(0);
}
