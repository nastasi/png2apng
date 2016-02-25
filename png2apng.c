#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <subpng.h>

int main(int argc, char *argv[])
{
    char *foutname;
    struct png_image *png_out, *png_in;
    struct png_acTL *actl;
    struct png_fcTL fctl_in, *fctl;
    struct png_IEND *iend;
    struct png_chunk *pc;

    int i, idat_idx;

    strcpy(fctl_in.type, "fcTL");
    fctl_in.sequence_number = 0;
    fctl_in.x_offset = 0;
    fctl_in.y_offset = 0;
    fctl_in.dispose_op = 1; // clean before apply new frame
    fctl_in.blend_op = 0;   // no blend, full replace

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
            actl = png_acTL_create(argc - 2, 0);
            if ((pc = png_acTL_dump(actl)) == NULL)
                exit(4);
            png_chunk_add(png_out, pc, actl);
        }
        fctl_in.sequence_number = i - 2;
        fctl_in.width = png_width(png_in);
        fctl_in.height = png_height(png_in);
        fctl_in.delay_num = 50;
        fctl_in.delay_den = 1000;

        if ((fctl = png_fcTL_create(&fctl_in)) == NULL)
            exit(7);
        if ((pc = png_fcTL_dump(fctl)) == NULL)
            exit(8);
        png_chunk_add(png_out, pc, fctl);

        idat_idx = png_chunk_by_type(png_in, "IDAT", 0);

        png_chunk_add_from_png(png_out, png_in, idat_idx);
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
