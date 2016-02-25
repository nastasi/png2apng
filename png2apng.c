#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <subpng.h>

int main(int argc, char *argv[])
{
    char *foutname;
    struct png_image *png_out, *png_in;
    struct png_acTL *actl;
    struct png_IEND *iend;
    struct png_chunk *pc;

    int i;

    foutname = argv[1];

    if (fopen(foutname, "rb")) {
        fprintf(stderr, "File %s already exists.\n", argv[1]);
        exit(1);
    }

    if ((png_out = png_create()) == NULL) {
        exit(3);
    }

    for (i = 2 ; i < argc ; i++) {
        if ((png_in = png_load(argv[i])) == FALSE)
            exit(3);

        if (i == 2) {
            png_chunk_add_from_png(png_out, png_in, 0);
            actl = png_acTL_create(argc - 1, 0);
            if ((pc = png_acTL_dump(actl)) == NULL)
                exit(4);
            png_chunk_add(png_out, pc, actl);
        }
    }

    if ((iend = png_IEND_create()) == NULL)
        exit (5);
    if ((pc = png_IEND_dump(iend)) == NULL)
        exit(6);

    png_chunk_add(png_out, pc, iend);

    png_print(png_out);

    png_write(png_out, foutname);

    printf("Finished\n");
    exit(0);
}
