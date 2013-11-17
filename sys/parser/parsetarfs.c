#include <sys/kstdio.h>
#include <defs.h>
#include <sys/parser/tarfs.h>
#include <sys/parser/parsetarfs.h>

int getsize(char *);
int strcmp(const char *,const char *);
int check_element_of_dir(const char* file, const char* dir);
int check_in_dir(const char* file, const char* dir);
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
		printf("file: %s, type: %s size: %d linkname: %s ", header->name, header->typeflag, size, header->mtime);
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

int open(const char* pathname){
	printf("pathname: %s\n", pathname);
	struct posix_header_ustar *header;
	uint64_t header_address = (uint64_t)&_binary_tarfs_start;
	uint64_t i;
	int fd;
	for (i = 0; ; i++){
		header = (struct posix_header_ustar *)header_address;
		if (strcmp(header->name,pathname)==0){
			printf("file found: %s \n", pathname);
			break;

		}
		else if (header->name[0] == '\0')
			return -1;
		int size = getsize(header->size);
		header_address += ((size / 512) + 1) * 512;
		if (size % 512)
			header_address += 512;
	}
	for(i =0; i<ULIMIT; i++){
		if(process_open_files_table[i].header == 0){
			process_open_files_table[i].header = header;	
			process_open_files_table[i].offset = 0;
			fd = i;
			break;
		}
	}
	if(i==ULIMIT){
		printf(" Number of open files exceeds ULIMIT");
		return -1;
	}
	return fd;
}

int read(int fd, void *buf, uint64_t count){
	int i = -1;
	if(process_open_files_table[fd].header == 0)
		return i;
	char *filetype = "0";
	char *dirtype = "5";
	printf("type: %s\n",process_open_files_table[fd].header->typeflag);
	if(strcmp(process_open_files_table[fd].header->typeflag,dirtype)==0){
		printf("Reading a directory");
	}
	else if(strcmp(process_open_files_table[fd].header->typeflag,filetype)==0){
		uint64_t header_address = (uint64_t)process_open_files_table[fd].header;
		uint64_t size_of_file = getsize(process_open_files_table[fd].header->size);
		uint64_t readptr = header_address + 512 + process_open_files_table[fd].offset;
		char *temp = buf;
		for(i =0;i<count;i++){
			if(size_of_file==process_open_files_table[fd].offset){
				*temp = *(char *)readptr;
				break;
			}
			*temp++ = *(char *)readptr++;
			process_open_files_table[fd].offset++;
		}	
	}
	return i;
}

int close(int fd){
	if(process_open_files_table[fd].header != 0){
		process_open_files_table[fd].header = 0;
		process_open_files_table[fd].offset = 0;
		return 0;
	}
	else
		return -1;
}

int opendir(const char *pathname){
	printf("pathname: %s\n", pathname);
	struct posix_header_ustar *header;
	uint64_t header_address = (uint64_t)&_binary_tarfs_start;
	uint64_t i;
	int fd;
	for (i = 0; ; i++){
		header = (struct posix_header_ustar *)header_address;
		if (strcmp(header->name,pathname)==0){
			char *dirtype = "5";
			if(strcmp(header->typeflag,dirtype)==0){
				printf("Found directory: %s \n ", pathname);
				break;	
			}
			else{	
				printf("%s not a directory\n", pathname);
				return -1;
			}	
		}
		else if (header->name[0] == '\0')
			return -1;
		int size = getsize(header->size);
		header_address += ((size / 512) + 1) * 512;
		if (size % 512)
			header_address += 512;
	}
	for(i =0; i<ULIMIT; i++){
		if(process_open_files_table[i].header == 0){
			process_open_files_table[i].header = header;	
			process_open_files_table[i].offset = 0;
			fd = i;
			break;
		}
	}
	if(i==ULIMIT){
		printf("ULIMIT exceeded\n");
		return -1;
	}
	return fd;
}

int check_in_dir(const char* file, const char* dir){
	while ( *file==*dir ){
	 	file++;
		dir++;
	}
	if( *dir!='\0')
		return -1;
	else{
		while(*file!='\0'){
			file++;
			if(*file=='/'){
				file++;
				break;
			}	
		}
		if (*file=='\0')
			return 0;
		else
			return 1;
	}
}

struct posix_header_ustar* readdir(int fd){
	if(process_open_files_table[fd].header == 0)
		return 0;
	char *dirtype = "5";
	if(strcmp(process_open_files_table[fd].header->typeflag,dirtype)==0){
		uint64_t header_address = (uint64_t)process_open_files_table[fd].header;
		for (uint64_t j = 0;j<=process_open_files_table[fd].offset ; j++){
			struct posix_header_ustar *header = (struct posix_header_ustar *)header_address;
			if (header->name[0] == '\0')
				break;
			int size = getsize(header->size);
			header_address += ((size / 512) + 1) * 512;
			if (size % 512)
				header_address += 512;
		
			struct posix_header_ustar *return_header = (struct posix_header_ustar *)header_address;
			if(check_in_dir(return_header->name,process_open_files_table[fd].header->name)==-1){
				return 0;
			}	
			else if(check_in_dir(return_header->name,process_open_files_table[fd].header->name)==1){
				j--;
			}	
			else{
				if(j==process_open_files_table[fd].offset){
					process_open_files_table[fd].offset++;
					return return_header;

				}
				else
					continue;
			}
		}
	}
	else{
		printf("Not a directory");
		return 0;
	}
	return 0;
}


int closedir(int fd){
	int i = close(fd);
	return i;
}

int lseek(int fd, off64_t offset, int whence) {

    if(!process_open_files_table[fd].header) {
        // No such file!
        return -1;
    }

    switch(whence) {
    case SEEK_SET:
        process_open_files_table[fd].offset = offset;
        break;
    case SEEK_CUR:
        process_open_files_table[fd].offset += offset;
        break;
    case SEEK_END:
        // TODO: Seek from end of file
        process_open_files_table[fd].offset = offset;
        break;
    }

    return 0;
}

