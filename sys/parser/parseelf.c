#include <defs.h>
#include <sys/parser/parsetarfs.h>
#include <sys/parser/parseelf.h>
#include <sys/kstdio.h>


void read_elf_header(int fd, Elf64_Ehdr *elf_header){
	printf("%d",sizeof(Elf64_Ehdr));
	read(fd, (void *)elf_header, sizeof(Elf64_Ehdr));
}

int is_elf(Elf64_Ehdr elf_header){
	printf("e_ident%p\n",elf_header.e_ident[EI_NIDENT]);
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
	printf("%d",sizeof(Elf64_Phdr));
	read(fd, (void *)prgm_header, sizeof(Elf64_Phdr));
	
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
