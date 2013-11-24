#include <sys/memory/page_constants.h>
#include <sys/memory/phys_page_manager2.h>
#include <sys/memory/free_phys_pages.h>
#include <sys/list.h>
#include <sys/memory/kmalloc.h>

LIST_HEAD(phys_free_list_head);

/* Physical memory chunks defined in phys_page_manager2.c */
extern struct list_head chunk_list_head;

int init_free_phys_page_manager() {

    struct chunk_t *node;
    int create_new_obj = 1;
    struct phys_free_list_t *phys_free_node = NULL;
    int retval = 0, i;
    uint64_t addr;

    list_for_each_entry(node, &chunk_list_head, chunk_list) {

        for(addr = node->phys_addr_start, i = 0; addr < node->phys_addr_end;
                                                 addr += SIZEOF_PAGE, i ++) {

            if(((node->phys_page_pointer)[i])->refcount > 0) {

                if(phys_free_node) {
                    list_add_tail(&phys_free_node->phys_free_list,
                                                        &phys_free_list_head);
                }

                create_new_obj = 1;
                phys_free_node = NULL;
                continue;
            }

            if(!create_new_obj) {
                phys_free_node->count ++;
                continue;
            }

            if(NULL != (phys_free_node = (struct phys_free_list_t*)
                                    kmalloc(sizeof(struct phys_free_list_t)))) {
                phys_free_node->addr = addr;
                phys_free_node->count = 1;
            } else {
                retval = -1;
                goto out;
            }

            create_new_obj = 0;
        }

out:
        if(phys_free_node) {
            list_add_tail(&phys_free_node->phys_free_list,
                                                        &phys_free_list_head);
        }

        create_new_obj = 1;
        phys_free_node = NULL;
    }

    return retval;
}

uint64_t alloc_phys_pages(uint32_t n) {

    struct phys_free_list_t *node;
    uint64_t ret_addr = 0;

    if(!n) {
        return 0;
    }

    list_for_each_entry(node, &phys_free_list_head, phys_free_list) {
        if(node->count < n) {
            continue;
        }

        ret_addr = node->addr;

        if(n == node->count) {
            list_del(&node->phys_free_list);
            kfree(node);
            break;
        }

        node->addr += (uint64_t)n * SIZEOF_PAGE;
        node->count -= n;
        break;
    }

    if(ret_addr) {
        inc_ref_count_pages(ret_addr, ret_addr + (uint64_t)n * SIZEOF_PAGE);
    }

    return ret_addr;
}

int free_phys_page(uint64_t addr) {

    int ref = dec_ref_count_page(addr);

    if(-1 == ref) {
        return -1;
    } else if (ref > 0) {
        return 0;
    }

    struct phys_free_list_t *next, *phys_free_node;
    struct phys_free_list_t *prev = NULL;

    list_for_each_entry(next, &phys_free_list_head, phys_free_list) {
        if(addr < next->addr)
            break;

        prev = next;
    }

    if(&next->phys_free_list == &phys_free_list_head)
        next = NULL;

    int merge = 0;

    // Merge with prev if possible
    if(prev && (prev->addr + (uint64_t)prev->count * SIZEOF_PAGE == addr)) {

        prev->count ++;
        merge = 1;
    }

    // Merge with next if possible
    if(next && (next->addr == addr + SIZEOF_PAGE)) {

        if(merge) {
            // Merge current and prev with next
            prev->count += next->count;
            list_del(&next->phys_free_list);
            kfree(next);
        } else {
            // Merge current with next
            merge = 1;
            next->addr = addr;
            next->count ++;
        }
    }

    if(merge)
        return 0;

    // Create new phys free node
    if(NULL != (phys_free_node = (struct phys_free_list_t*)
                                    kmalloc(sizeof(struct phys_free_list_t)))) {
        phys_free_node->addr = addr;
        phys_free_node->count = 1;

        struct list_head *prev_list_head = prev? &prev->phys_free_list :
                                                        &phys_free_list_head;
        struct list_head *next_list_head = next? &next->phys_free_list :
                                                        &phys_free_list_head;

        __list_add(&phys_free_node->phys_free_list, prev_list_head,
                                                                next_list_head);
        return 0;
    }

    return -1;
}

extern char kernmem;

uint64_t v_alloc_page_get_phys_at_virt(uint64_t *phys_addr, uint64_t prot,
                                                                uint64_t virt) {
    uint64_t paddr = alloc_phys_pages(1);

    if(!paddr) {
        return 0;
    }

    uint64_t virt_addr;
    virt_addr = mmap(&(get_kernel_mm()->mmap), virt, SIZEOF_PAGE, prot,
                                                        MAP_ANONYMOUS, 0, 0);

    if(virt_addr == 0) {
        free_phys_page(paddr);
        return 0;
    }

    // TODO: Status check and free_phys_page if not ok
    update_curr_page_table(paddr, virt_addr, prot);

    if(phys_addr) {
        *phys_addr = paddr;
    }

    return virt_addr;
}

uint64_t v_alloc_page_get_phys(uint64_t *phys_addr, uint64_t prot) {
    return v_alloc_page_get_phys_at_virt(phys_addr, prot, (uint64_t)(&kernmem));
}

uint64_t v_alloc_pages_at_virt(uint32_t n, uint64_t prot, uint64_t virt) {

    if(n == 0) {
        return 0;
    }

    uint64_t paddr[n];
    int free_count;
    uint64_t virt_addr;

    for(int i = 0; i < n; i ++) {
        paddr[i] = alloc_phys_pages(1);

        if(!paddr[i]) {
            virt_addr = 0;
            free_count = i;
            goto out;
        }
    }

    virt_addr = mmap(&(get_kernel_mm()->mmap), virt, n * SIZEOF_PAGE, prot,
                                                        MAP_ANONYMOUS, 0, 0);

    if(virt_addr == 0) {
        virt_addr = 0;
        free_count = n;
        goto out;
    }

    // TODO: Status check and free_phys_page if not ok
    for(int i = 0; i < n; i ++) {
        update_curr_page_table(paddr[i], virt_addr + i * SIZEOF_PAGE, prot);
    }

out:
    free_count = 0;

    for(int i = 0; i < free_count; i ++) {
        free_phys_page(paddr[i]);
    }

    return virt_addr;
}

uint64_t v_alloc_pages(uint32_t n, uint64_t prot) {

    return v_alloc_pages_at_virt(n, prot, (uint64_t)(&kernmem));
}

int v_free_page(uint64_t virt) {
    if(!virt)
        return -1;

    uint64_t perm;
    uint64_t phys = virt2phys_selfref(virt, &perm);

    if(phys == (uint64_t)-1) {
        return -1;
    }

    update_curr_page_table(0, virt, perm);
    // TODO: munmap

    return free_phys_page(phys);
}

