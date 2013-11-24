#ifndef _FILE_STRUCTURES_H
#define _FILE_STRUCTURES_H
#include <defs.h>

#define TERMINAL_TYPE 1
#define TARFS_TYPE 2
#define FS_TYPE 3

struct posix_header_ustar {
	char name[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char checksum[8];
	char typeflag[1];
	char linkname[100];
	char magic[6];
	char version[2];
	char uname[32];
	char gname[32];
	char devmajor[8];
	char devminor[8];
	char prefix[155];
	char pad[12];
};

struct process_files_table{
	struct posix_header_ustar *header;
	uint64_t offset;
};

struct process_files_table* get_new_process_files_table(
				struct posix_header_ustar *header,
				uint64_t offset);


#endif
