#include <sys/memory/setup_kernel_memory.h>
#include <sys/memory/mm_struct.h>
#include <sys/memory/phys_page_manager2.h>
#include <sys/memory/free_phys_pages.h>
#include <sys/memory/handle_cr2_cr3.h>

/**
 * Create vma for kernel virtual memory
 * @param kern_start starting virtual address of kernel
 * @param kern_end   ending virtual address of kernel
 * @return           OK or ERROR
 */
static int set_kernel_memory(uint64_t kern_start, uint64_t kern_end) {

    struct kernel_mm_struct *mm = get_kernel_mm();

    // TODO: Replace with error code
    if(-1 == add_vma(&(mm->mmap), kern_start, kern_end, PAGE_TRANS_READ_WRITE,
                                                                        0)) {
        return -1;
    }

    mm->start_kernel = kern_start;
    mm->end_kernel = kern_end;

    // TODO: Replace with error code
    return 0;
}

/**
 * Create vma for video buffer memory
 * @param vdo_buff_start starting virtual address of video buffer
 * @param vdo_buff_end   ending virtual address of video buffer
 * @return               OK or ERROR
 */
static int set_video_buffer_memory(uint64_t vdo_buff_start,
                                                        uint64_t vdo_buff_end) {

    struct kernel_mm_struct *mm = get_kernel_mm();

    // TODO: Replace with error code
    if(-1 == add_vma(&(mm->mmap), vdo_buff_start, vdo_buff_end,
                                                    PAGE_TRANS_READ_WRITE, 0)) {
        return -1;
    }

    mm->start_vdo_buff = vdo_buff_start;
    mm->end_vdo_buff = vdo_buff_end;

    // TODO: Replace with error code
    return 0;
}

/**
 * Create vma for virtual map of physical memory
 * @param phys_mem_start starting virtual address of physical memory map
 * @param phys_mem_end   ending virtual address of physical memory map
 * @return               OK or ERROR
 */
static int set_phys_memory(uint64_t phys_mem_start, uint64_t phys_mem_end) {

    struct kernel_mm_struct *mm = get_kernel_mm();

    // TODO: Replace with error code
    if(-1 == add_vma(&(mm->mmap), phys_mem_start, phys_mem_end,
        PAGE_TRANS_READ_WRITE | PAGE_TRANS_NX, 0)) {
        return -1;
    }

    mm->start_phys_mem = phys_mem_start;
    mm->end_phys_mem = phys_mem_end;

    // TODO: Replace with error code
    return 0;

}

int setup_kernel_memory(uint64_t kernmem, uint64_t p_kern_start,
                        uint64_t p_kern_end, uint64_t p_vdo_buff_start,
                        uint32_t *modulep) {

    struct kernel_mm_struct *mm = get_kernel_mm();

    // Set up vma
    // Kernel virtual memory space
    if(-1 == set_kernel_memory(kernmem , kernmem - p_kern_start
                                                              + p_kern_end)) {
        return -1;
    }
    // Video buffer memory
    // TODO: Check return value
    uint64_t vdo_start_addr = get_unmapped_area(&(mm->mmap),
                                kernmem + p_vdo_buff_start, SIZEOF_PAGE);
    if(-1 == set_video_buffer_memory(vdo_start_addr, vdo_start_addr
                                                            + SIZEOF_PAGE)) {
        return -1;
    }

    // Scan physical pages
    struct smap_t {
        uint64_t base, length;
        uint32_t type;
    }__attribute__((packed)) *smap;

    uint64_t phys_end_addr = 0;
    int lower_chunk = 0;
    uint64_t lower_chunk_start = 0;
    uint64_t lower_chunk_end = 0;

    while(modulep[0] != 0x9001) modulep += modulep[1]+2;
    for(smap = (struct smap_t*)(modulep+2); smap <
                (struct smap_t*)((char*)modulep + modulep[1] + 2*4); ++smap) {
        if (smap->type == 1 && smap->length != 0) {

            if(phys_end_addr < smap->base + smap->length) {
                phys_end_addr = smap->base + smap->length;
            }

            if(!lower_chunk) {
                lower_chunk_start = smap->base;
                lower_chunk_end = smap->base + smap->length;
                lower_chunk ++;
            }

            if(!new_chunk(smap->base, smap->base + smap->length)) {
                return -1;
            }
		}
	}

    // TODO: Check return value
    uint64_t phys_mem_offset = get_unmapped_area(&(mm->mmap), kernmem,
                                                                 phys_end_addr);
    if(-1 == set_phys_memory(phys_mem_offset, phys_mem_offset
                                                            + phys_end_addr)) {
        return -1;
    }

    if(-1 == scan_all_chunks()) {
        return -1;
    }

    // Mark used physical pages
    // The first page - just like that
    if(0 > inc_ref_count_pages(0, SIZEOF_PAGE)) {
        return -1;
    }
    // Video buffer memory - is not part of chunks obtained from modulep. No
    //                       need to mark.
    // Kernel physical pages
    if(0 > inc_ref_count_pages(p_kern_start, p_kern_end)) {
        return -1;
    }
    // Ignore lower chunk
    if(0 > inc_ref_count_pages(lower_chunk_start, lower_chunk_end)) {
        return -1;
    }

    // Initialize free pages
    if(-1 == init_free_phys_page_manager()) {
        return -1;
    }

    printf("start kernel: %p\n", mm->start_kernel);
    printf("end kernel  : %p\n", mm->end_kernel);
    printf("start vdo   : %p\n", mm->start_vdo_buff);
    printf("end vdo     : %p\n", mm->end_vdo_buff);
    printf("start phys  : %p\n", mm->start_phys_mem);
    printf("end phys    : %p\n", mm->end_phys_mem);

    // Set up page tables
    uint64_t pml4_page = get_selfref_PML4(NULL);

	uint64_t paddr = p_kern_start;
	uint64_t vaddr = kernmem;

    while(paddr < p_kern_end) {
        update_page_table_idmap(pml4_page, paddr, vaddr, PAGE_TRANS_READ_WRITE);
        paddr += SIZEOF_PAGE;
        vaddr += SIZEOF_PAGE;
    }

    // TODO: Remove user supervisor permission from video buffer
    update_page_table_idmap(pml4_page, p_vdo_buff_start, vdo_start_addr,
                            PAGE_TRANS_READ_WRITE | PAGE_TRANS_USER_SUPERVISOR);

    phys_mem_offset_map(pml4_page, phys_mem_offset);

    // Set cr3
    struct str_cr3 cr3 = get_default_cr3();
    cr3.p_PML4E_4Kb = pml4_page >> 12;
    set_cr3(cr3);

    // Indicate memory set up done
    kmDeviceMemorySetUpDone();
    global_video_vaddr = (void *)vdo_start_addr;
    set_phys_mem_virt_map_base(phys_mem_offset);

    return 0;
}

