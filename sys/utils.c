#include <sys/utils.h>
#include "kstring.h"

/* Ref: http://tekpool.wordpress.com/category/bit-count/ */
int numOfBitsSet(const void *pBitmap, int bytes) {
    int bitsCnt = 0, bytesIndex;
    unsigned char l_pBitmap[16]; // TODO: As of now max bytes is 8
    memcpy(l_pBitmap, pBitmap, bytes);
    unsigned char *ptr = (unsigned char*) l_pBitmap;
    for(bytesIndex = 0; bytesIndex < bytes; bytesIndex ++) {
        ptr[bytesIndex] -= ( ((ptr[bytesIndex] >> 1) & 0x77)
                           + ((ptr[bytesIndex] >> 2) & 0x33)
                           + ((ptr[bytesIndex] >> 3) & 0x11) );
        bitsCnt += ptr[bytesIndex] % 15;
    }
    return bitsCnt;
}


