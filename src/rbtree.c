#include "ds/rbtree.h"

static inline void rotate_left(struct rb_node *node)
{
	struct rb_node *new = node->right;
	node->right = new->left;
	new->left = node;
	rb_set_parent(new, rb_parent(node));
	rb_set_parent(node, new);
}

static inline void rotate_right(struct rb_node *node)
{
	struct rb_node *new = node->left;
	node->left = new->right;
	new->right = node;
	rb_set_parent(new, rb_parent(node));
	rb_set_parent(node, new);
}

static inline void insert_case1(struct rb_node *node)
{
	rb_set_color(node, RB_BLACK);
}

static inline void insert_case2(struct rb_node *node)
{
	(void)node;
}

static inline void insert_case3(struct rb_node *node)
{
	rb_set_color(rb_parent(node), RB_BLACK);
	rb_set_color(rb_uncle(node), RB_BLACK);
	rb_set_color(rb_grandparent(node), RB_RED);
	rb_insert_color(node);	
}

static inline void insert_case4(struct rb_node *node)
{
	struct rb_node *parent = rb_parent(node), *grandparent = rb_grandparent(node);

	if (node == grandparent->left->right) {
		rotate_left(parent);
		node = node->left;
	} else if (node == grandparent->right->left) {
		rotate_right(parent);
		node = node->right;
	}

	if (node == parent->left) {
		rotate_right(grandparent);
	} else {
		rotate_left(grandparent);
	}

	rb_set_color(parent, RB_BLACK);
	rb_set_color(parent, RB_RED);
}

void rb_insert_color(struct rb_node *node)
{
	if (node == NULL)
		return;

	struct rb_node *parent = rb_parent(node);
	if (parent == NULL)
		insert_case1(node);
	else if (rb_color(parent) == RB_BLACK)
		insert_case2(node);
	else if (rb_color(rb_uncle(node)) == RB_RED)
		insert_case3(node);
	else
		insert_case4(node);
}
