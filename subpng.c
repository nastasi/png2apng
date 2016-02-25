#include "subpng.h"

/*
  4 (type + content) len
  4 type
  N content
  4 CRC (calculated on
*/
static unsigned char subpng_ref_head[] = { 137, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A };

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

int png_chunk_write(FILE *fp, struct png_chunk *pc)
{
    char *bf[4];

    *((uint32_t *)bf) = htonl(pc->len);
    if (fwrite(bf, 1, 4, fp) != 4)
        return FALSE;

    if (fwrite(pc->content, 1, pc->len + 4 , fp) != pc->len + 4)
        return FALSE;

    *((uint32_t *)bf) = htonl(pc->crc);

    if (fwrite(bf, 1, 4, fp) != 4)
        return FALSE;

    return TRUE;
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

struct png_acTL *png_acTL_create(uint32_t num_frames, uint32_t num_plays)
{
    struct png_acTL *ret;

    if ((ret = (struct png_acTL *)malloc(sizeof(struct png_acTL))) == NULL) {
        return NULL;
    }
    strcpy(ret->type, "fcTL");

    ret->num_frames = num_frames;
    ret->num_plays = num_plays;

    return (ret);
}

struct png_chunk *png_acTL_dump(struct png_acTL *actl)
{
    struct png_chunk *pc = NULL;
    uint8_t *bf = NULL;
    uLong crc;

    do {
        if ((pc = calloc(1, sizeof(struct png_chunk))) == NULL) {
            return NULL;
        }
        pc->len = 8;

        if ((bf = malloc(pc->len + 4)) == NULL) {
            return NULL;
        }

        strcpy(pc->type, "acTL");

        memcpy(&(bf[0]), "acTL", 4);
        *((uint32_t *)(&bf[4])) = htonl(actl->num_frames);
        *((uint32_t *)(&bf[8])) = htonl(actl->num_plays);

        crc = crc32(0L, Z_NULL, 0);
        crc = crc32(crc, &(bf[0]), pc->len + 4);

        pc->content = bf;
        pc->data = &(pc->content[4]);
        pc->crc = crc;

        return pc;
    } while (0);

    if (pc)
        free(pc);
    if (bf)
        free(bf);
    return NULL;
}

struct png_acTL *png_acTL_read(struct png_chunk *pc)
{
    struct png_acTL *ret;

    if ((ret = (struct png_acTL *)malloc(sizeof(struct png_acTL))) == NULL) {
        return NULL;
    }
    strcpy(ret->type, "acTL");

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
struct png_fcTL *png_fcTL_create(struct png_fcTL *fctl_in)
{
    struct png_fcTL *ret;

    if ((ret = (struct png_fcTL *)malloc(sizeof(struct png_fcTL))) == NULL) {
        return NULL;
    }
    strcpy(ret->type, "fcTL");
    if (fctl_in) {
        memcpy(ret, fctl_in, sizeof(struct png_fcTL));
    }

    return ret;
}

struct png_fcTL *png_fcTL_read(struct png_chunk *pc)
{
    struct png_fcTL *ret;

    if ((ret = png_fcTL_create(NULL)) == NULL) {
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

struct png_chunk *png_fcTL_dump(struct png_fcTL *fctl)
{
    struct png_chunk *pc = NULL;
    uint8_t *bf = NULL;
    uLong crc;

    do {
        if ((pc = calloc(1, sizeof(struct png_chunk))) == NULL) {
            return NULL;
        }
        pc->len = 26;

        if ((bf = malloc(pc->len + 4)) == NULL) {
            return NULL;
        }
        strcpy(pc->type, "fcTL");
        memcpy(&(bf[0]), "fcTL", 4);
        *((uint32_t *)(&bf[4])) = htonl(fctl->sequence_number);
        *((uint32_t *)(&bf[8])) = htonl(fctl->width);
        *((uint32_t *)(&bf[12])) = htonl(fctl->height);
        *((uint32_t *)(&bf[16])) = htonl(fctl->x_offset);
        *((uint32_t *)(&bf[20])) = htonl(fctl->y_offset);
        *((uint16_t *)(&bf[24])) = htons(fctl->delay_num);
        *((uint16_t *)(&bf[26])) = htons(fctl->delay_den);
        *((uint8_t *)(&bf[28])) = fctl->dispose_op;
        *((uint8_t *)(&bf[29])) = fctl->blend_op;

        crc = crc32(0L, Z_NULL, 0);
        crc = crc32(crc, &(bf[0]), pc->len + 4);

        pc->content = bf;
        pc->data = &(pc->content[4]);
        pc->crc = crc;

        return pc;
    } while (0);

    if (pc)
        free(pc);
    if (bf)
        free(bf);
    return NULL;
}




void png_fcTL_print(struct png_fcTL *fctl)
{
    printf("fcTL: seq_num: %d  w: %d  h: %d  delay: %d/%d\n",
           fctl->sequence_number, fctl->width, fctl->height, fctl->delay_num, fctl->delay_den);
}




struct png_IEND *png_IEND_create(void)
{
    struct png_IEND *ret;

    if ((ret = (struct png_IEND *)malloc(sizeof(struct png_IEND))) == NULL) {
        return NULL;
    }
    strcpy(ret->type, "IEND");

    return (ret);
}

struct png_chunk *png_IEND_dump(struct png_IEND *iend)
{
    struct png_chunk *pc = NULL;
    uint8_t *bf = NULL;
    uLong crc;

    do {
        if ((pc = calloc(1, sizeof(struct png_chunk))) == NULL) {
            return NULL;
        }

        if ((bf = malloc(20)) == NULL) {
            return NULL;
        }

        pc->len = 0;
        strcpy(pc->type, "IEND");

        memcpy(&(bf[0]), "IEND", 4);

        crc = crc32(0L, Z_NULL, 0);
        crc = crc32(crc, &(bf[0]), 4);

        pc->content = bf;
        pc->data = &(pc->content[4]);
        pc->crc = crc;

        return pc;
    } while (0);

    if (pc)
        free(pc);
    if (bf)
        free(bf);
    return NULL;
}

int png_IEND_write(FILE *fp, struct png_IEND *iend)
{
    char *bf[64];
    struct png_chunk *pc;

    if ((pc = png_IEND_dump(iend)) == NULL) {
        return FALSE;
    }

    *((uint32_t *)(&bf[0])) = htonl(pc->len);
    memcpy(&(bf[4]), pc->content, pc->len + 4);
    *((uint32_t *)(&bf[pc->len + 8])) = htonl(pc->crc);

    if (fwrite(bf, 1, pc->len + 8, fp) != pc->len + 8)
        return FALSE;

    return TRUE;
}

struct png_image *png_create(void)
{
    return (calloc(1, sizeof(struct png_image)));
}

int png_chunk_add(struct png_image *png, struct png_chunk *pc, void *spec)
{
    if (spec == NULL) {
        if (strcmp(pc->type, "IHDR") == 0) {
            if ((spec = (void *)png_IHDR_read(pc)) == NULL) {
                return FALSE;
            }
        }
        else if (strcmp(pc->type, "acTL") == 0) {
            if ((spec = (void *)png_acTL_read(pc)) == NULL) {
                return FALSE;
            }
        }
        else if (strcmp(pc->type, "fcTL") == 0) {
            if ((spec = (void *)png_fcTL_read(pc)) == NULL) {
                return FALSE;
            }
        }
        else {
            spec = NULL;
        }
    }

    png->chunk[png->chunk_n] = pc;
    png->chunk_data[png->chunk_n] = spec;
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
    unsigned char head[8];
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

        if (memcmp(head, subpng_ref_head, 8) != 0) {
            break;
        }

        do {
            if ((pc = png_chunk_read(fp)) == NULL) {
                failure = TRUE;
                break;
            }
            png_chunk_add(ret, pc, NULL);
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

int png_write(struct png_image *png, char *filename)
{
    FILE *fp;
    int i;

    if ((fp = fopen(filename, "wb")) == NULL)
        return FALSE;

    if (fwrite(subpng_ref_head, 1, 8, fp) != 8)
        return FALSE;

    for (i = 0 ; i < png->chunk_n ; i++) {
        png_chunk_write(fp, png->chunk[i]);
    }

    fclose(fp);
    return TRUE;
}

int png_width(struct png_image *png)
{
    int i;
    for (i = 0 ; i < png->chunk_n ; i++) {
        if (strcmp(png->chunk_type[i], "IHDR") == 0) {
            return ((struct png_IHDR *)png->chunk_data[i])->w;
        }
    }
    return -1;
}

int png_height(struct png_image *png)
{
    int i;
    for (i = 0 ; i < png->chunk_n ; i++) {
        if (strcmp(png->chunk_type[i], "IHDR") == 0) {
            return ((struct png_IHDR *)png->chunk_data[i])->h;
        }
    }
    return -1;
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

int png_chunk_by_type(struct png_image *png, char *type, int start_id)
{
    int i;

    for (i = start_id ; i < png->chunk_n ; i++) {
        if (strcmp(png->chunk_type[i], type) == 0) {
            return i;
        }
    }
    return -1;
}
