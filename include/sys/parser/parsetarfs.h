#ifndef _PARSETARFS_H
#define _PARSETARFS_H
/* parses tarfs and prints the names of files
* returns the number of files in tarfs
*/
int parsetar();

/* reads @numbytes from @offset of @filename into @buffer
* note: does not handle the case when size of @buffer is less than @numbytes
*/
int read_tarfs( char *filename, uint64_t offset, uint64_t numbytes, char * buffer);

#endif
