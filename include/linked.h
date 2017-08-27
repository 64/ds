#pragma once

#include <stddef.h>

struct slist_entry {
	struct slist_entry *next;
};

// Compiler magic to find the original struct (taken from linux kernel)
#define container_of(ptr, type, member) ({                      \
        const typeof(((type *)0)->member) *__mptr = (ptr);    \
        (type *)((char *)__mptr - offsetof(type, member)); })

#define slist_entry(ptr, type, member) \
	container_of(ptr, type, member)

#define slist_get_next(ptr, member) ({ \
	const struct slist_entry *__next = (ptr)->member.next; \
	__next == NULL ? NULL : slist_entry(__next, typeof(*(ptr)), member); })

#define slist_set_next(ptr, member, next_entry) ({ \
	const typeof(next_entry) __next_entry = (next_entry); \
	struct slist_entry *__head = &(ptr)->member; \
	struct slist_entry *__next = (__next_entry == NULL) ? NULL : &__next_entry->member; \
	__head->next = __next; })

#define slist_foreach(head, member) \
	for (; (head) != NULL; (head) = slist_get_next((head), member))
