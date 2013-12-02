#include <sys/kstdio.h>
#include <defs.h>
#include <sys/parser/tarfs.h>
#include <sys/parser/parsetarfs.h>
#include <sys/filesystems/file_structures.h>
#include <sys/scheduler/scheduler.h>
#include <sys/scheduler/pcb.h>


int getsize(char *);
int strcmp(const char *,const char *);
int check_element_of_dir(const char* file, const char* dir);
int check_in_dir(const char* file, const char* dir);
int closefd(int fd);

struct operation_pointers tarfs_ops = {
	sys_open,
	sys_read,
	NULL,
	sys_lseek,
	sys_close,
	sys_opendir,
	sys_readdir,
	sys_closedir
};
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
//			printf("file found: %s \n", filename);
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

int sys_open(const char* pathname){
//	printf("pathname: %s\n", pathname);
	struct posix_header_ustar *header;
	uint64_t header_address = (uint64_t)&_binary_tarfs_start;
	struct process_files_table* pft;
	uint64_t i, fd;
	for (i = 0; ; i++){
		header = (struct posix_header_ustar *)header_address;
		if (strcmp(header->name,pathname)==0){
//			printf("file found: %s \n", pathname);
			break;

		}
		else if (header->name[0] == '\0'){
//			printf("Sys open failed]\n");
			return -1;
		}	
		int size = getsize(header->size);
		header_address += ((size / 512) + 1) * 512;
		if (size % 512)
			header_address += 512;
	}
	pft = get_new_process_files_table(header,0,get_tarfs_ops()); //TODO: Do we need offset?
	fd = add_to_process_file_table(getCurrentTask(),pft);
//	printf("\nFd returned from sys_open:%d",fd);
	return fd;
}

int64_t sys_read(int fd, void *buf, uint64_t count){
	int64_t i = 0;
	struct process_files_table* pft = get_process_files_table(getCurrentTask(),fd);
	if(pft == NULL){
		return -1;
	}
	char *filetype = "0";
	char *dirtype = "5";

	if(strcmp(pft->header->typeflag,dirtype)==0){
		printf("Reading a directory");
	}
	else if(strcmp(pft->header->typeflag,filetype)==0){
		uint64_t header_address = (uint64_t)pft->header;
		uint64_t size_of_file = getsize(pft->header->size);
		uint64_t readptr = header_address + 512 + pft->offset;
		char *temp = (char *)buf;
		for(i =0;i<count;i++){
			if(size_of_file==pft->offset){
				*temp = *(char *)readptr;
				break;
			}
			*temp++ = *(char *)readptr++;
			pft->offset++;
		}	
	}
	return i;
}

int sys_close(int fd){
	return closefd(fd);
}

int sys_opendir(const char *pathname){
//	printf("pathname: %s\n", pathname);
	struct posix_header_ustar *header;
	uint64_t header_address = (uint64_t)&_binary_tarfs_start;
	struct process_files_table* pft;
	uint64_t fd, i;
	for (i = 0; ; i++){
		header = (struct posix_header_ustar *)header_address;
		if (strcmp(header->name,pathname)==0){
			char *dirtype = "5";
			if(strcmp(header->typeflag,dirtype)==0){
//				printf("Found directory: %s \n ", pathname);
				break;	
			}
			else{	
//				printf("%s not a directory\n", pathname);
				return NULL;
			}	
		}
		else if (header->name[0] == '\0')
			return -1;
		int size = getsize(header->size);
		header_address += ((size / 512) + 1) * 512;
		if (size % 512)
			header_address += 512;
	}
	pft = get_new_process_files_table(header,0,get_tarfs_ops());
	fd = add_to_process_file_table(getCurrentTask(),pft);
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

struct posix_header_ustar* sys_readdir(int fd, uint64_t ret){
	struct posix_header_ustar* to_ret = (struct posix_header_ustar*)ret;
	struct process_files_table* pft = get_process_files_table(getCurrentTask(),fd);
	if(pft == NULL)
		return NULL;
	char *dirtype = "5";
	if(strcmp(pft->header->typeflag,dirtype)==0){
		uint64_t header_address = (uint64_t)pft->header;
		for (uint64_t j = 0;j<= pft->offset ; j++){
			struct posix_header_ustar *header = (struct posix_header_ustar *)header_address;
			if (header->name[0] == '\0')
				break;
			int size = getsize(header->size);
			header_address += ((size / 512) + 1) * 512;
			if (size % 512)
				header_address += 512;
		
			struct posix_header_ustar *return_header = (struct posix_header_ustar *)header_address;
			if(check_in_dir(return_header->name,pft->header->name)==-1){
				return 0;
			}	
			else if(check_in_dir(return_header->name,pft->header->name)==1){
				j--;
			}	
			else{
				if(j==pft->offset){
					pft->offset++;
					to_ret = return_header;
					return to_ret;

				}
				else
					continue;
			}
		}
	}
	else{
		printf("Not a directory");
		return NULL;
	}
	return 0;
}


int sys_closedir(int fd){
	return closefd(fd);
}

int closefd(int fd){
	return reset_process_files_table(getCurrentTask(), fd);
}

int sys_lseek(int fd, off64_t offset, int whence) {

	struct process_files_table* pft = get_process_files_table(getCurrentTask(),fd);
    if(pft == NULL) {
        // No such file!
        return -1;
    }

    switch(whence) {
    case SEEK_SET:
        pft->offset = offset;
        break;
    case SEEK_CUR:
        pft->offset += offset;
        break;
    case SEEK_END:
        // TODO: Seek from end of file
        pft->offset = offset;
        break;
    }

    return 0;
}

struct operation_pointers* get_tarfs_ops(){
	return &tarfs_ops;
}
