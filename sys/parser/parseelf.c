#include <defs.h>
#include <sys/parser/parsetarfs.h>
#include <sys/parser/parseelf.h>
#include <sys/kstdio.h>
#include <sys/memory/mm_struct.h>

void read_elf_header(int fd, Elf64_Ehdr *elf_header){
	
	sys_read(fd, (void *)elf_header, sizeof(Elf64_Ehdr));
}

int is_elf(Elf64_Ehdr elf_header){
	//printf("e_ident%p\n",elf_header.e_ident[EI_NIDENT]);
	//char *magic = "\177ELF";
	if(elf_header.e_ident[0]==0x7f){
		if(elf_header.e_ident[1]=='E'){
			if(elf_header.e_ident[2]=='L'){
				if(elf_header.e_ident[3]=='F')
					return 0;
			}
		}
	}
	printf("not an elf file");
	return -1;
}

void print_elf_header(Elf64_Ehdr elf_header){
	printf("entry:%p\n", elf_header.e_entry);
	printf("programheadertable:%p\n", elf_header.e_phoff);
	printf("sectionheadertable:%p\n", elf_header.e_shoff);
	printf("headersize::%p\n", elf_header.e_ehsize);
	printf("programheadersize:%p\n", elf_header.e_phentsize);
	printf("ph_num_entries:%p\n", elf_header.e_phnum);
	printf("sh_size:%p\n", elf_header.e_shentsize);
	printf("sh_num_entries:%p\n", elf_header.e_shnum);
	printf("index:%p\n", elf_header.e_shstrndx);

}

void read_prgm_header(int fd, Elf64_Phdr *prgm_header){
	//printf("%d",sizeof(Elf64_Phdr));
	sys_read(fd, (void *)prgm_header, sizeof(Elf64_Phdr));
	
}

void print_prgm_header(Elf64_Phdr prgm_header){
	printf("p_type: %p\n",prgm_header.p_type);
	printf("p_flags: %p\n",prgm_header.p_flags);
	printf("p_offset: %p\n",prgm_header.p_offset);
	printf("p_vaddr: %p\n",prgm_header.p_vaddr);
	printf("p_paddr: %p\n",prgm_header.p_paddr);
	printf("p_filesz: %p\n",prgm_header.p_filesz);
	printf("p_memsz: %p\n",prgm_header.p_memsz);
	printf("p_align: %p\n",prgm_header.p_align);
}


uint64_t load_elf(struct mm_struct *this, char* filename){

	Elf64_Ehdr elf_header ;
	uint64_t entry = -1;
	int fd = sys_open(filename);
	//printf("\nfd opened in loadelf: %d",fd);
	if(fd==-1)
		return entry;
	read_elf_header(fd, &elf_header);
	//print_elf_header(elf_header);
	if(is_elf(elf_header)==0){
		//print_elf_header(elf_header);
		uint16_t num_ph = elf_header.e_phnum;
		entry = elf_header.e_entry;
		Elf64_Phdr prgm_header[num_ph];
		for(int num_phdr =0;num_phdr < num_ph;num_phdr++){
			read_prgm_header(fd, &(prgm_header[num_phdr]));
			//print_prgm_header(prgm_header[num_phdr]);
		}
		for(int num_phdr =0;num_phdr < num_ph;num_phdr++){
			if(prgm_header[num_phdr].p_type==0x1){
				uint64_t offset = prgm_header[num_phdr].p_offset;
				uint64_t size = prgm_header[num_phdr].p_filesz;
				uint64_t vir_addr = prgm_header[num_phdr].p_vaddr;
				uint64_t flag =  prgm_header[num_phdr].p_flags;
				uint64_t prot = PAGE_TRANS_USER_SUPERVISOR;
			//	if(!(flag & PT_E))
			//		prot = prot | PAGE_TRANS_NX;
				if(flag & PT_W)
					prot = prot | PAGE_TRANS_READ_WRITE;

				int mmap_return = do_mmap(&(this->mmap), fd, offset, vir_addr, size, prot);
				if(mmap_return != 0){
					sys_close(fd);
					return -1;
				}	

			}
		}

	}
	sys_close(fd);
	return entry; 

}	
