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
#include <stdint.h>
#include <zlib.h>
#include <arpa/inet.h>

#ifndef SUBPNG_H
#define SUBPNG_H 1

#ifndef FALSE
#define FALSE 0
#define TRUE (~FALSE)
#endif

struct png_chunk {
    uint32_t len;
    uint8_t *content;
    char type[5];
    uint8_t *data;
    uint32_t crc;
};

struct png_IHDR {
    char type[5];
    uint32_t w;
    uint32_t h;
    uint8_t depth;
    uint8_t ctype;
    uint8_t filter;
    uint8_t interl;
};

struct png_acTL {
    char type[5];
    uint32_t num_frames;
    uint32_t num_plays;
};

struct png_IEND {
    char type[5];
};

/*
  from wiki.mozilla.org
*/
struct png_fcTL {
    char type[5];
    uint32_t sequence_number; /* Sequence number of the animation chunk, starting from 0 */
    uint32_t width;           /* Width of the following frame */
    uint32_t height;          /* Height of the following frame */
    uint32_t x_offset;        /* X position at which to render the following frame */
    uint32_t y_offset;        /* Y position at which to render the following frame */
    uint16_t delay_num;       /* Frame delay fraction numerator */
    uint16_t delay_den;       /* Frame delay fraction denominator */
    uint8_t dispose_op;       /* Type of frame area disposal to be done after rendering this frame */
    uint8_t blend_op;         /* Type of frame area rendering for this frame */
};

#define SUBPNG_MAXCHUNK 1024

struct png_img {
    void *chunk[SUBPNG_MAXCHUNK];
    char chunk_type[SUBPNG_MAXCHUNK][5];
    void *chunk_data[SUBPNG_MAXCHUNK];
    int chunk_n;
};

struct png_chunk *png_chunk_read(FILE *fp);
int               png_chunk_write(FILE *fp, struct png_chunk *pc);
void              png_chunk_print(struct png_chunk *pc);

struct png_IHDR  *png_IHDR_read(struct png_chunk *pc);
void              png_IHDR_print(struct png_IHDR *ihdr);

struct png_acTL  *png_acTL_create(uint32_t num_frames, uint32_t num_plays);
struct png_acTL  *png_acTL_read(struct png_chunk *pc);
struct png_chunk *png_acTL_dump(struct png_acTL *actl);
void              png_acTL_print(struct png_acTL *acTL);

struct png_fcTL  *png_fcTL_create(struct png_fcTL *in);
struct png_fcTL  *png_fcTL_read(struct png_chunk *pc);
struct png_chunk *png_fcTL_dump(struct png_fcTL *fctl);
void              png_fcTL_print(struct png_fcTL *fctl);

struct png_IEND  *png_IEND_create(void);
struct png_chunk *png_IEND_dump(struct png_IEND *iend);

struct png_chunk *png_fdAT_from_IDAT(struct png_chunk *pc_in, uint32_t seq);

struct png_img *png_img_create(void);
struct png_img *png_img_load(char *fname);
int             png_img_write(struct png_img *png, char *filename);
void            png_img_print(struct png_img *png);
int             png_img_chunk_add_from_png(struct png_img *png_out, struct png_img *png_in, int id);
int             png_img_chunk_add(struct png_img *png, struct png_chunk *pc, void *spec);
int             png_img_chunk_by_type(struct png_img *png, char *type, int start_id);
int             png_img_width(struct png_img *png);
int             png_img_height(struct png_img *png);
#endif
