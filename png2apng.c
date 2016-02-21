#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <zlib.h>
#include <arpa/inet.h>

struct png_chunk {
    uint32_t len;
    unsigned char *content;
    unsigned char *id;
    unsigned char *data;
    uint32_t crc;
};

int chunk_read(FILE *fp)
{
    uLong crc;
    struct png_chunk *pc;
  
    if ((pc = malloc(sizeof(struct png_chunk))) == NULL) {
        return (-1);
    }

    if (fread(&(pc->len), 1, 4, fp) != 4)
        return (-1);
    pc->len = ntohl(pc->len);

    if ((pc->content = (unsigned char *)malloc(pc->len + 4)) == NULL) {
        return (-2);
    }
    
    if (fread(pc->content, 1, pc->len + 4, fp) != (pc->len + 4))
        return (-3);
 
    pc->id = pc->content;
    pc->data = &(pc->content[4]);

    if (fread(&(pc->crc), 1, 4, fp) != 4)
        return (-4);
    pc->crc = ntohl(pc->crc);

    crc = crc32(0L, Z_NULL, 0);
    crc = crc32(crc, pc->content, pc->len + 4);

    printf("LEN: %u\n", pc->len);
    printf("ID:  %c%c%c%c\n", pc->id[0], pc->id[1], pc->id[2], pc->id[3]);
    printf("CRC EXP: %u  OUT: %lu\n", pc->crc, crc);

    return 0;
}

int main(int argc, char *argv[])
{
    FILE *fp;
    unsigned char head[8], ref_head[] = { 137, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A };
    int i;

    if ((fp = fopen(argv[1], "r")) == NULL)
        exit(1);
    
    if (fread(head, 1, 8, fp) != 8)
        exit(2);

    if (memcmp(head, ref_head, 8) != 0) {
        exit(3);
    }

    for (i = 0 ; i < 10 ; i++) {
        printf("\n");
        if (chunk_read(fp) != 0) {
            exit(4);
        }
    }

    exit(0);
}
