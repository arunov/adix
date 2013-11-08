#ifndef _LIST_H
#define _LIST_H

#include <defs.h>

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)


struct list_head{
	struct list_head * next;
	struct list_head * prev;
};

typedef struct my_struct{
	int data;
	struct list_head myList;
}mystruct;


/** 
* Insert a new entry between two known consecutive entries. 
*/ 
static inline void __list_add(struct list_head *new, struct list_head *prev, struct list_head *next){
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

/**
* list_add - add a new entry 
* @param new:        new entry to be added 
* @param head:       list head to add it after 
* 
* Insert a new entry after the specified head. 
* 
*/
static inline void list_add(struct list_head *new, struct list_head *head){
	__list_add(new, head, head->next);
}

/**
* list_add_tail - add a new entry
* @param new:        new entry to be added
* @param head:       list head to add it before
*
* Insert a new entry before the specified head.
* Useful for implementing queues.
*/
static inline void list_add_tail(struct list_head *new, struct list_head *head){
	__list_add(new, head->prev, head);
}

/**
* Delete a list entry by making the prev/next entries
* point to each other.
*
* This is only for internal list manipulation where we know
* the prev/next entries already!
*/
static inline void __list_del(struct list_head *prev, struct list_head *next){
	next->prev = prev;
	prev->next = next;
}

/**
* list_del - deletes entry from list.
* @param entry:      the element to delete from the list.
*/
static inline void list_del(struct list_head *entry){
	__list_del(entry->prev, entry->next);
	entry->next = (void *) 0;
	entry->prev = (void *) 0;
}


/**
* list_empty - tests whether a list is empty
* @param head:       the list to test.
*/
static inline int list_empty(struct list_head *head)
{
	return head->next == head;
}

/**
* list_for_each - iterate over a list 
* @param pos:        the &struct list_head to use as a loop counter. 
* @param head:       the head for your list. 
*/

#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)


#define offsetof(TYPE, MEMBER) ((uint64_t) &((TYPE *)0)->MEMBER)
/** 
* container_of - cast a member of a structure out to the containing structure 
* @ptr:        the pointer to the member. 
* @type:       the type of the container struct this is embedded in. 
* @member:     the name of the member within the struct. 
* 
*/ 
#define container_of(ptr, type, member) ({                      \
	const __typeof__( ((type *)0)->member ) *__mptr = (ptr);    \
	(type *)( (char *)__mptr - offsetof(type,member) );})

/** 
* list_entry - get the struct for this entry 
* @ptr:        the &struct list_head pointer. 
* @type:       the type of the struct this is embedded in. 
* @member:     the name of the list_struct within the struct. 
*/ 
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)



/** 
* list_for_each_entry  -       iterate over list of given type 
* @pos:        the type * to use as a loop counter. 
* @head:       the head for your list. 
* @member:     the name of the list_struct within the struct. 
*/ 
#define list_for_each_entry(pos, head, member)                          \
	for (pos = list_entry((head)->next, __typeof__(*pos), member);      \
		&pos->member != (head);        \
		pos = list_entry(pos->member.next, __typeof__(*pos), member))


#endif
