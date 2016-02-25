#include <subpng.h>

int main(int argc, char *argv[])
{
    struct png_image *png;

    if ((png = png_load(argv[1])) == NULL)
        exit(1);

    printf("W: %d H: %d\n\n", png_width(png), png_height(png));
    png_print(png);

    exit(0);
}

