#include <subpng.h>

int main(int argc, char *argv[])
{
    struct png_img *png;

    if ((png = png_img_load(argv[1])) == NULL)
        exit(1);

    printf("W: %d H: %d\n\n", png_img_width(png), png_img_height(png));
    png_img_print(png);

    exit(0);
}

