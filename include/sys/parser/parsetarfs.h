#ifndef _PARSETARFS_H
#define _PARSETARFS_H
/* parses tarfs and prints the names of files
* returns the number of files in tarfs
*/
#define ULIMIT 1024

int ulimit; 

struct process_files_table{
	struct posix_header_ustar *header;
	uint64_t offset;
};
struct process_files_table process_open_files_table[ULIMIT];

int parsetar();

/* reads @numbytes from @offset of @filename into @buffer
* note: does not handle the case when size of @buffer is less than @numbytes
*/
int read_tarfs( char *filename, uint64_t offset, uint64_t numbytes, char * buffer);

int open(const char* pathname);

int read(int fd, void *buf, uint64_t count);

int close(int fd);

int opendir(const char *pathname);

struct posix_header_ustar* readdir(int fd);

int closedir(int fd);

#endif
