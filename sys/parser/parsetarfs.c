#include <sys/kstdio.h>
#include <defs.h>
#include <sys/parser/tarfs.h>

int getsize(char *);
int strcmp(const char *,const char *);

int parsetar(){
	printf("inside parsetar tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
	uint64_t header_address = (uint64_t)&_binary_tarfs_start;
	printf("tar starting at %p\n", header_address );
	int i;
	for (i = 0; ; i++){
		struct posix_header_ustar *header = (struct posix_header_ustar *)header_address;
		if (header->name[0] == '\0')
			break;
		int size = getsize(header->size);
		header_address += ((size / 512) + 1) * 512;
		if (size % 512)
			header_address += 512;
	}
	return i;
}

int getsize(char *in){
	int size = 0;
	int j;
	int count = 1;
	for (j = 11; j > 0; j--, count *= 8)
		size += ((in[j - 1] - '0') * count);
	return size;
}

int read_tarfs(char *filename, uint64_t offset, uint64_t numbytes, char *buffer){
	uint64_t header_address = (uint64_t)&_binary_tarfs_start;
	uint64_t i;
	for (i = 0; ; i++){
		struct posix_header_ustar *header = (struct posix_header_ustar *)header_address;
		if (strcmp(header->name,filename)==0){
			printf("file found: %s \n", filename);
			break;
		}
		else if (header->name[0] == '\0')
			return -1;
		int size = getsize(header->size);
		header_address += ((size / 512) + 1) * 512;
		if (size % 512)
			header_address += 512;
	}
	uint64_t readptr = header_address + 512 + offset;
	char *temp = buffer;
	for(i =0;i<numbytes;i++){
		if(*(char *)readptr== '\0'){
			*temp = *(char *)readptr;
			break;
		}
		*temp++ = *(char *)readptr++;
	}
	return i;
}

int strcmp(const char *s1,const char *s2){
	while ( *s1==*s2 ){
		if( *s1=='\0' )
			return 0;
		s1++;
		s2++;
	}
	return *s1-*s2;
}
