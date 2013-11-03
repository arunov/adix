#include <sys/kstdio.h>
#include <defs.h>
#include <sys/tarfs.h>

int getsize(char *);


int parsetar(){
	printf("inside parsetar tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
	uint64_t header_address = (uint64_t)&_binary_tarfs_start;

//	struct posix_header_ustar *headers[64];
	printf("tar starting at %p\n", header_address );
	int i;

	for (i = 0; ; i++)
	{
		struct posix_header_ustar *header = (struct posix_header_ustar *)header_address;
		printf(" header name at %s\n", header->name);
		if (header->name[0] == '\0')
			break;
		int size = getsize(header->size);
//		headers[i] = header;
		header_address += ((size / 512) + 1) * 512;
		if (size % 512)
			header_address += 512;
	}
	return i;
	

}

int getsize(char *in)
{
	int size = 0;
	int j;
	int count = 1;
	for (j = 11; j > 0; j--, count *= 8)
		size += ((in[j - 1] - '0') * count);
	return size;
}
