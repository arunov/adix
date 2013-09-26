#ifndef _UTILS_H
#define _UTILS_H

/**
 * Obtain number of bits set in input bitmap of size 'bytes'.
 * @author         Arun Olappamanna Vasudevan
 * @email          aolappamanna@cs.stonybrook.edu
 * @param pnum     Pointer to bitmap
 * @param bytes    Number of bytes
 * @return         Number of bits set in bitmap
 */
int numOfBitsSet(const void *pBitmap, int bytes);

#endif

