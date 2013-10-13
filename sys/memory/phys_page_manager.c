#include <sys/memory/phys_page_manager.h>

void phys_page_manager_init(struct phys_page_manager *this,
                            uint32_t *a_modulep, void *a_physbase,
                            void *a_physfree) {

    this->free_list_root = a_physfree;
    this->free_list_last = 0;
    this->modulep = a_modulep;
    this->physbase = a_physbase;
    this->physfree = a_physfree;

    // Leave one page for creating free_lists
    this->scan_pos = (void*)(((uint64_t)a_physfree & PG_SZ_MASK)
                                                        + PG_SZ);
    this->i_max_fl_nd = (this->scan_pos - a_physfree)
                        /sizeof(struct phys_page_node);

    // If too less number of pages can be accomodated, then take 
    // one more page for free_list
    if(this->i_max_fl_nd < MIN_INITIAL_PAGES){
        this->i_max_fl_nd +=
                            PG_SZ/sizeof(struct phys_page_node);
        this->scan_pos += PG_SZ;
    }

    phys_page_scan(this, MODE_INIT, a_physfree);

    /*this->scan_state = SCAN_OFF;

    while((this->modulep)[0] != 0x9001)
        (this->modulep) += (this->modulep)[1] + 2;

    // Scanning for current scan position to end of memory
    for(this->smap = (struct smap_t*)(this->modulep + 2);
        this->smap < (struct smap_t*)((char*)((this->modulep)
                        + (this->modulep)[1] + 2*4));
        ++(this->smap)) {

        if(this->smap->type != 1 || this->smap->length == 0)
            continue;

        void *start = (void*)this->smap->base;
        start = ((uint64_t)start & PG_ALN_CHK_MASK)?
                (void*)(((uint64_t)start & PG_SZ_MASK) + PG_SZ)
                :(void*)start;
        void *end = (void*)(this->smap->base + this->smap->length);
        end = (void*)((uint64_t)end & PG_SZ_MASK);

        // Wait till we reach current scan position
        if(this->scan_state == SCAN_OFF &&
            (this->scan_pos < start || this->scan_pos >= end))
            continue;

        // Physfree reached. Scan starts... and continues...
        this->scan_state = SCAN_ON;

        // Check if we'll exceed max number of nodes
        uint32_t new_nodes = (end-(this->scan_pos))>>PG_SZ_BITS;

        if(new_nodes > this->i_max_fl_nd) {

            // Oh no! we exceed!
            end -= (new_nodes-(this->i_max_fl_nd))*PG_SZ;
        }
        this->n_nodes += create_free_phys_pages(this,
                            this->scan_pos, end);

        this->scan_pos = end;

        if(this->n_nodes == this->i_max_fl_nd)
            return;
    }

    // Scanning from start to*/ 
}

void phys_page_scan(struct phys_page_manager *this,
                    int mode, void *p_node) {

    if(this->scan_state == SCAN_DONE)
        return;

    uint32_t n_scans;
    uint32_t scan;
    uint32_t scan_pos_reset = 0;
    int l_scan_state;

    if((uint64_t)this->scan_pos > (uint64_t)this->physfree) {
        n_scans = 2; // 2nd scan needed from start to physbase
        scan_pos_reset = 0; // Scan position not reset
    } else {
        n_scans = 1;
        scan_pos_reset = 1; // Scan position is reset
    }

    int last_iter = 0;

    struct phys_page_node *p_next_node =
                                (struct phys_page_node*) p_node;

    for(scan = 0; scan < n_scans; scan ++) {

        l_scan_state = SCAN_OFF;

        while((this->modulep)[0] != 0x9001)
            (this->modulep) += (this->modulep)[1] + 2;

        for(this->smap = (struct smap_t*)(this->modulep + 2);
            (this->smap < (struct smap_t*)((char*)(this->modulep)
                + (this->modulep)[1] + 2*4)) && !last_iter;
            ++(this->smap)) {

            if(this->smap->type != 1 || this->smap->length == 0)
                continue;

            // Start of chunk
            void *start = (void*)this->smap->base;
            start = ((uint64_t)start & PG_ALN_CHK_MASK)?
                (void*)(((uint64_t)start & PG_SZ_MASK) + PG_SZ)
                :(void*)start;

            // End of chunk
            void *end = (void*)(this->smap->base
                            + this->smap->length);
            end = (void*)((uint64_t)end & PG_SZ_MASK);

            // If second scan, do only till physbase
            if(end > this->physbase && scan == 1) {
                end = this->physbase;
                last_iter = 1;
            }

            // If scan position is not reset yet and this is 2nd
            // scan
            if(scan_pos_reset == 0 && scan == 1) {
                scan_pos_reset = 1;
                this->scan_pos = start;
            }
            // Wait for reaching current scan position
            else if(l_scan_state == SCAN_OFF &&
                    this->scan_pos >= end)
                continue;

            // Reached... what are we waiting for...
            l_scan_state = SCAN_ON;

            // Check if scan position is within chunk
            if(this->scan_pos < start)
                this->scan_pos = start;

            void *new_end = start;

            while(new_end < end) {

                // Did we run out of space?
                if(this->n_nodes == this->i_max_fl_nd) {
                    switch(mode) {
                    case MODE_INIT: return;
                    case MODE_FULL:
                        p_next_node = (struct phys_page_node*)
                                        get_free_phys_page(this);
                        if(p_next_node == 0)
                            return;
                        this->i_max_fl_nd +=
                            PG_SZ/sizeof(struct phys_page_node);
                    }
                }

                // Number of new nodes
                uint32_t new_nodes = (end-(this->scan_pos))
                                    >>PG_SZ_BITS;
                new_end = end;

                // Check if we exceed the number of nodes
                if(this->n_nodes + new_nodes > this->i_max_fl_nd) {

                    // Oh no! we exceed!
                    new_end = end -
                                (this->n_nodes + new_nodes
                                - (this->i_max_fl_nd)) * PG_SZ;
                }

                new_nodes = create_free_phys_pages(this,
                                    this->scan_pos, new_end,
                                    p_next_node);

                this->n_nodes += new_nodes;
                p_next_node += new_nodes;
                this->scan_pos = new_end;
            }
        }
    }
}

void finish_scan(struct phys_page_manager *this) {
    phys_page_scan(this, MODE_FULL, 0);
}

void* get_free_phys_page(struct phys_page_manager *this) {
    void *free_phys_page = 0;

    if(this->free_list_root) {
        free_phys_page = this->free_list_root->addr;
        this->free_list_root = this->free_list_root->next;
    }

    return free_phys_page;
}

void* add_free_phys_page(struct phys_page_manager *this,
                            void *addr);

uint32_t create_free_phys_pages(struct phys_page_manager *this,
                            void *start, void *end,
                            struct phys_page_node *p_node) {
    void *ptr;
    uint32_t n_nodes = 0;

    struct phys_page_node *node = p_node;

    for(ptr = start; ptr < end; ptr += PG_SZ) {
        if(this->free_list_last) {
            this->free_list_last->next = node;
        }

        node->addr = ptr;
        node->next = 0;
        node->prev = this->free_list_last;
        this->free_list_last = node;
        node ++;
        n_nodes ++;
    }

    return n_nodes;
}

