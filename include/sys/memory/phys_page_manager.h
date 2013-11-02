#ifndef _PHYS_PAGE_MANAGER_H
#define _PHYS_PAGE_MANAGER_H

#include <defs.h>

struct phys_page_node {
    void *addr;
    struct phys_page_node *next;
    struct phys_page_node *prev;
};

enum {
    SCAN_ON,
    SCAN_OFF,
    SCAN_DONE,
    MODE_INIT,
    MODE_FULL,
};

#define PG_SZ 0x1000
#define PG_SZ_BITS 12
#define PG_SZ_MASK 0xfffffffffffff000UL
#define PG_ALN_CHK_MASK 0xfffUL

#define MIN_INITIAL_PAGES 100

struct phys_page_manager {
    struct phys_page_node *free_list_root;
    struct phys_page_node *free_list_last;

    uint32_t *modulep;
    void *physbase;
    void *physfree;

    void *scan_pos;
    uint32_t i_max_fl_nd; // initial max free list nodes
    uint32_t n_nodes;
    int scan_state;

    struct smap_t {
        uint64_t base, length;
        uint32_t type;
    } __attribute__((packed)) *smap;

};

void phys_page_manager_init(struct phys_page_manager *this,
                            uint32_t *a_modulep, void *a_physbase,
                            void *a_physfree);

void finish_scan(struct phys_page_manager *this);

void* get_free_phys_page(struct phys_page_manager *this);

void* add_free_phys_page(struct phys_page_manager *this,
                            void *addr);

uint32_t create_free_phys_pages(struct phys_page_manager *this,
                            void *start, void *end,
                            struct phys_page_node *p_node);

void phys_page_scan(struct phys_page_manager *this, int mode,
                    void *p_node);

struct phys_page_manager *getPhysPageManager();

#endif

