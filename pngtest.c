#include <subpng.h>

int main(int argc, char *argv[])
{
    FILE *fp;
    unsigned char head[8], ref_head[] = { 137, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A };
    struct png_chunk *pc;
    struct png_IHDR *ihdr;
    struct png_acTL *actl;
    struct png_fcTL *fctl;


    if ((fp = fopen(argv[1], "r")) == NULL)
        exit(1);

    if (fread(head, 1, 8, fp) != 8)
        exit(2);

    if (memcmp(head, ref_head, 8) != 0) {
        exit(3);
    }

    do {
        if ((pc = png_chunk_read(fp)) == NULL) {
            exit(4);
        }

        if (strcmp(pc->id, "IHDR") == 0) {
            if ((ihdr = png_IHDR_read(pc)) == NULL) {
                exit(5);
            }
            png_IHDR_print(ihdr);
        }
        else if (strcmp(pc->id, "acTL") == 0) {
            if ((actl = png_acTL_read(pc)) == NULL) {
                exit(6);
            }
            png_acTL_print(actl);
        }
        else if (strcmp(pc->id, "fcTL") == 0) {
            if ((fctl = png_fcTL_read(pc)) == NULL) {
                exit(6);
            }
            png_fcTL_print(fctl);
        }
        else {
            printf("%s (len: %d)\n", pc->id, pc->len);
        }
    } while (strcmp(pc->id, "IEND") != 0);

    exit(0);
}

