#include <syscall.h>
#include <stdio.h>

char *state_list[3] = {"running", "waiting", "waiting_timer"};

int main() {
	printf("\n In main");
    struct ps_t *list;
    process_snapshot(&list);
    struct ps_t *scan = list;
    printf("(pid) state process\n");
    printf("-------------------\n");
    while(scan) {
        printf("(%p) %s", scan->pid, state_list[(int)(scan->state)]);
        if(scan->name) {
            printf(" %s", scan->name);
        }
        printf("\n");
        scan = scan->next;
    }
    free_ps_list(&list);
    return 0;
}

