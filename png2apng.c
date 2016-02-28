/*
    Copyright (C) 2016 Matteo Nastasi <nastasi@alternativeoutput.it>

    This file is part of png2apng.

    png2apng is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <subpng.h>

int filename2msec(char *s);

int filename2msec(char *s)
{
    char bf[32], *b, *e;

    if ((e = strstr(s, "ms)")) == NULL) {
        return 1000;
    }
    for (b = (e - 1) ; b >= s ; b--) {
        if (*b == '(') {
            memcpy(bf, b+1, e - b - 1);
            bf[e - b - 1] = '\0';
            break;
        }
    }
    return atoi(bf);
}

int main(int argc, char *argv[])
{
    char *foutname;
    struct png_img *png_out, *png_in;
    struct png_acTL *actl;
    struct png_fcTL fctl_in, *fctl;
    struct png_IEND *iend;
    struct png_chunk *pc;

    int i, idat_idx, seq = 0;

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

    if ((png_out = png_img_create()) == NULL) {
        exit(2);
    }

    for (i = 2 ; i < argc ; i++) {
        if ((png_in = png_img_load(argv[i])) == FALSE)
            exit(3);

        if (i == 2) {
            png_img_chunk_add_from_png(png_out, png_in, 0);
            actl = png_acTL_create(argc - 2, 0);
            if ((pc = png_acTL_dump(actl)) == NULL)
                exit(4);
            png_img_chunk_add(png_out, pc, actl);
        }
        fctl_in.sequence_number = seq++;
        fctl_in.width = png_img_width(png_in);
        fctl_in.height = png_img_height(png_in);
        fctl_in.delay_num = filename2msec(argv[i]);
        fctl_in.delay_den = 1000;

        if ((fctl = png_fcTL_create(&fctl_in)) == NULL)
            exit(7);
        if ((pc = png_fcTL_dump(fctl)) == NULL)
            exit(8);
        png_img_chunk_add(png_out, pc, fctl);

        idat_idx = png_img_chunk_by_type(png_in, "IDAT", 0);
        pc = png_in->chunk[idat_idx];

        if (i > 2) {
            pc = png_fdAT_from_IDAT(pc, seq++);
            png_img_chunk_add(png_out, pc, NULL);
        }
        else {
            png_img_chunk_add(png_out, pc, NULL);
        }
    }

    if ((iend = png_IEND_create()) == NULL)
        exit (5);
    if ((pc = png_IEND_dump(iend)) == NULL)
        exit(6);

    png_img_chunk_add(png_out, pc, iend);

    png_img_print(png_out);

    png_img_write(png_out, foutname);

    printf("Finished\n");
    exit(0);
}

