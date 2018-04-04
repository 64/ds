#pragma once

#include <stddef.h>
#include <stdint.h>

#include "generic.h"

struct rb_node {
	uintptr_t parent_color;
	struct rb_node *left;
	struct rb_node *right;
} __attribute__((aligned((sizeof(long)))));

struct rbtree {
	struct rb_node *root; // Root of the tree
	struct rb_node *most_left; // The furthest left node in the tree
};

#define rb_entry(ptr, type, member) \
	container_of(ptr, type, member)

#define rb_entry_safe(ptr, type, member) \
	({ const struct rb_node *__ptr = (ptr); \
	__ptr == NULL ? rb_entry(__ptr, type, member) : NULL; })

#define RB_PARENT_MASK (~1)
#define RB_COLOR_MASK (~RB_PARENT_MASK)
#define RB_BLACK 0
#define RB_RED 1

#define rb_mask_parent(ptr) \
	((uintptr_t)(ptr) & RB_PARENT_MASK)
#define rb_mask_color(ptr) \
	((uintptr_t)(ptr) & RB_COLOR_MASK)

#define rb_is_red(node) \
	(node != NULL && rb_mask_color((node)->parent_color) == RB_RED)
#define rb_is_black(node) \
	(node == NULL || rb_mask_color((node)->parent_color) == RB_BLACK)

static inline struct rb_node *rb_parent(struct rb_node *node)
{
	return (struct rb_node *)rb_mask_parent(node->parent_color);
}

static inline unsigned int rb_color(struct rb_node *node)
{
	return rb_mask_color(node->parent_color);	
}

static inline struct rb_node *rb_grandparent(struct rb_node *node)
{
	struct rb_node *parent = rb_parent(node);
	if (parent == NULL)
		return NULL;
	return rb_parent(parent);
}

static inline struct rb_node *rb_sibling(struct rb_node *node)
{
	struct rb_node *parent = rb_parent(node);	
	if (parent == NULL)
		return NULL;
	else if (node == parent->left)
		return parent->right;
	else
		return parent->left;
}

static inline struct rb_node *rb_uncle(struct rb_node *node)
{
	struct rb_node *parent = rb_parent(node);	
	struct rb_node *grandparent = rb_grandparent(node);
	if (grandparent == NULL)
		return NULL;
	return rb_sibling(parent);
}

static inline void rb_set_color(struct rb_node *node, unsigned int color)
{
	if (color == 1)
		node->parent_color |= 1;
	else if (color == 0)
		node->parent_color &= ~1;
}

static inline void rb_set_parent(struct rb_node *node, struct rb_node *parent)
{
	unsigned int color = rb_color(node);
	node->parent_color = (uintptr_t)parent | color;
}

void rb_insert_color(struct rb_node *node);
