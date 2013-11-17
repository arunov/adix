#include <defs.h>
#include <sys/parser/parsetarfs.h>
#include <sys/parser/parseelf.h>
#include <sys/kstdio.h>

int exec(char *filename){

	Elf64_Ehdr elf_header ;
	int fd = open(filename);
	read_elf_header(fd, &elf_header);
	if(is_elf(elf_header)==0){
		print_elf_header(elf_header);
		uint16_t num_ph = elf_header.e_phnum;
		Elf64_Phdr prgm_header[num_ph];
		for(int num_phdr =0;num_phdr < num_ph;num_phdr++){
			read_prgm_header(fd, &(prgm_header[num_phdr]));
			print_prgm_header(prgm_header[num_phdr]);
		}
		for(int num_phdr =0;num_phdr < num_ph;num_phdr++){
			if(prgm_header[num_phdr].p_type==0x1){
				uint64_t size = prgm_header[num_phdr].p_filesz;
				uint64_t vir_addr = prgm_header[num_phdr].p_vaddr;
				int filedesc = open(filename);
				// do an mmap and do a read
				printf("prgm head loadable %d size%p vir_addr%p filedesc %d\n", num_phdr, size, vir_addr, filedesc );
			
			}
		}

	}
	else
		return -1;
	close(fd);
	return 0; 

}	
