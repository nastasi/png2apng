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
    uint32_t w;
    uint32_t h;
    uint8_t depth;
    uint8_t ctype;
    uint8_t filter;
    uint8_t interl;
};

struct png_acTL {
    uint32_t num_frames;
    uint32_t num_plays;
};

/*
  from wiki.mozilla.org
*/
struct png_fcTL {
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

struct png_image {
    void *chunk[SUBPNG_MAXCHUNK];
    char chunk_type[SUBPNG_MAXCHUNK][5];
    void *chunk_data[SUBPNG_MAXCHUNK];
    int chunk_n;
};

struct png_chunk *png_chunk_read(FILE *fp);
void png_chunk_print(struct png_chunk *pc);
struct png_IHDR *png_IHDR_read(struct png_chunk *pc);
void png_IHDR_print(struct png_IHDR *ihdr);
struct png_acTL *png_acTL_read(struct png_chunk *pc);
void png_acTL_print(struct png_acTL *acTL);
struct png_fcTL *png_fcTL_read(struct png_chunk *pc);
void png_fcTL_print(struct png_fcTL *fctl);
struct png_image *png_create(void);
struct png_image *png_load(char *fname);
void png_print(struct png_image *png);
int png_chunk_add_from_png(struct png_image *png_out, struct png_image *png_in, int id);
#endif
