#include <sys/memory/vm_area_struct.h>
#include <sys/memory/kmalloc.h>

struct vm_area_struct* new_vma() {

    return (struct vm_area_struct*) kmalloc(sizeof(struct vm_area_struct));
}

