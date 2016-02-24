#include "subpng.h"

struct png_chunk *png_chunk_read(FILE *fp)
{
    uLong crc;
    struct png_chunk *pc;

    if ((pc = malloc(sizeof(struct png_chunk))) == NULL) {
        return (NULL);
    }

    if (fread(&(pc->len), 1, 4, fp) != 4)
        return (NULL);
    pc->len = ntohl(pc->len);

    if ((pc->content = (unsigned char *)malloc(pc->len + 4)) == NULL) {
        return (NULL);
    }

    if (fread(pc->content, 1, pc->len + 4, fp) != (pc->len + 4))
        return (NULL);

    memcpy(pc->type, pc->content, 4);
    pc->type[4] = '\0';
    pc->data = &(pc->content[4]);

    if (fread(&(pc->crc), 1, 4, fp) != 4)
        return (NULL);
    pc->crc = ntohl(pc->crc);

    crc = crc32(0L, Z_NULL, 0);
    crc = crc32(crc, pc->content, pc->len + 4);

    if (crc != pc->crc)
        return (NULL);

    return pc;
}

void png_chunk_print(struct png_chunk *pc)
{
    printf("%s: len: %d (unmanaged)\n", pc->type, pc->len);
}

struct png_IHDR *png_IHDR_read(struct png_chunk *pc)
{
    struct png_IHDR *ret;

    if ((ret = (struct png_IHDR *)malloc(sizeof(struct png_IHDR))) == NULL) {
        return NULL;
    }

    memcpy(&(ret->w), pc->data, 4);
    memcpy(&(ret->h), &(pc->data[4]), 4);
    ret->w = ntohl(ret->w);
    ret->h = ntohl(ret->h);

    memcpy(&(ret->depth), &(pc->data[8]), 1);
    memcpy(&(ret->ctype), &(pc->data[9]), 1);
    memcpy(&(ret->filter), &(pc->data[10]), 1);
    memcpy(&(ret->interl), &(pc->data[11]), 1);


    return (ret);
}


void png_IHDR_print(struct png_IHDR *ihdr)
{
    printf("IHDR: w: %d  h: %d  depth: %d  ctype: %d\n", ihdr->w, ihdr->h, ihdr->depth, ihdr->ctype);
}

struct png_acTL *png_acTL_read(struct png_chunk *pc)
{
    struct png_acTL *ret;

    if ((ret = (struct png_acTL *)malloc(sizeof(struct png_acTL))) == NULL) {
        return NULL;
    }

    memcpy(&(ret->num_frames), pc->data, 4);
    ret->num_frames = ntohl(ret->num_frames);

    memcpy(&(ret->num_plays), &(pc->data[4]), 4);
    ret->num_plays = ntohl(ret->num_plays);

    return (ret);
}

void png_acTL_print(struct png_acTL *acTL)
{
    printf("acTL: frames: %d  plays: %d\n", acTL->num_frames, acTL->num_plays);
}

/*
  fcTL chunks

  from mozilla wiki:
   byte
    0    sequence_number       (unsigned int)   Sequence number of the animation chunk, starting from 0
    4    width                 (unsigned int)   Width of the following frame
    8    height                (unsigned int)   Height of the following frame
   12    x_offset              (unsigned int)   X position at which to render the following frame
   16    y_offset              (unsigned int)   Y position at which to render the following frame
   20    delay_num             (unsigned short) Frame delay fraction numerator
   22    delay_den             (unsigned short) Frame delay fraction denominator
   24    dispose_op            (byte)           Type of frame area disposal to be done after rendering this frame
   25    blend_op              (byte)           Type of frame area rendering for this frame
*/
struct png_fcTL *png_fcTL_read(struct png_chunk *pc)
{
    struct png_fcTL *ret;

    if ((ret = (struct png_fcTL *)malloc(sizeof(struct png_fcTL))) == NULL) {
        return NULL;
    }

    memcpy(&(ret->sequence_number), &(pc->data[0]), 4);
    ret->sequence_number = ntohl(ret->sequence_number);

    memcpy(&(ret->width), &(pc->data[4]), 4);
    ret->width = ntohl(ret->width);

    memcpy(&(ret->height), &(pc->data[8]), 4);
    ret->height = ntohl(ret->height);

    memcpy(&(ret->x_offset), &(pc->data[12]), 4);
    ret->x_offset = ntohl(ret->x_offset);

    memcpy(&(ret->y_offset), &(pc->data[16]), 4);
    ret->y_offset = ntohl(ret->y_offset);

    memcpy(&(ret->delay_num), &(pc->data[20]), 2);
    ret->delay_num = ntohs(ret->delay_num);

    memcpy(&(ret->delay_den), &(pc->data[22]), 2);
    ret->delay_den = ntohs(ret->delay_den);

    memcpy(&(ret->dispose_op), &(pc->data[24]), 1);

    memcpy(&(ret->blend_op), &(pc->data[25]), 1);

    return ret;
}

void png_fcTL_print(struct png_fcTL *fctl)
{
    printf("fcTL: seq_num: %d  w: %d  h: %d  delay: %d/%d\n",
           fctl->sequence_number, fctl->width, fctl->height, fctl->delay_num, fctl->delay_den);
}

struct png_image *png_create(void)
{
    return (calloc(1, sizeof(struct png_image)));
}

int png_chunk_add(struct png_image *png, struct png_chunk *pc)
{
    void *chunk;

    if (strcmp(pc->type, "IHDR") == 0) {
        if ((chunk = (void *)png_IHDR_read(pc)) == NULL) {
            return FALSE;
        }
    }
    else if (strcmp(pc->type, "acTL") == 0) {
        if ((chunk = (void *)png_acTL_read(pc)) == NULL) {
            return FALSE;
        }
    }
    else if (strcmp(pc->type, "fcTL") == 0) {
        if ((chunk = (void *)png_fcTL_read(pc)) == NULL) {
            return FALSE;
        }
    }
    else {
        chunk = NULL;
    }
    png->chunk[png->chunk_n] = pc;
    png->chunk_data[png->chunk_n] = chunk;
    strcpy(png->chunk_type[png->chunk_n], pc->type);
    png->chunk_n++;

    return TRUE;
}

int png_chunk_add_from_png(struct png_image *png_out, struct png_image *png_in, int id)
{
    png_out->chunk[png_out->chunk_n] = png_in->chunk[id];
    strcpy(png_out->chunk_type[png_out->chunk_n], png_in->chunk_type[id]);
    png_out->chunk_data[png_out->chunk_n] = png_in->chunk_data[id];
    png_out->chunk_n++;

    return TRUE;
}

struct png_image *png_load(char *fname)
{
    FILE *fp = NULL;
    unsigned char head[8], ref_head[] = { 137, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A };
    int failure = FALSE;
    struct png_chunk *pc;

    struct png_image *ret;

    do {
        if ((ret = png_create()) == NULL)
            break;

        if ((fp = fopen(fname, "rb")) == NULL) {
            break;
        }

        if (fread(head, 1, 8, fp) != 8) {
            break;
        }

        if (memcmp(head, ref_head, 8) != 0) {
            break;
        }

        do {
            if ((pc = png_chunk_read(fp)) == NULL) {
                failure = TRUE;
                break;
            }
            png_chunk_add(ret, pc);
        } while (strcmp(pc->type, "IEND") != 0);

        if (failure) {
            break;
        }
        fclose(fp);

        // SUCCESS
        return ret;
    } while (0);

    if (fp)
        fclose(fp);

    return NULL;
}

void png_print(struct png_image *png)
{
    int i;
    for (i = 0 ; i < png->chunk_n ; i++) {
        if (strcmp(png->chunk_type[i], "IHDR") == 0) {
            png_IHDR_print((struct png_IHDR *)png->chunk_data[i]);
        }
        else if (strcmp(png->chunk_type[i], "acTL") == 0) {
            png_acTL_print((struct png_acTL *)png->chunk_data[i]);
        }
        else if (strcmp(png->chunk_type[i], "fcTL") == 0) {
            png_fcTL_print((struct png_fcTL *)png->chunk_data[i]);
        }
        else {
            png_chunk_print(png->chunk[i]);
        }
    }
}
