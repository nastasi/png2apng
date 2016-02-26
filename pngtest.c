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

