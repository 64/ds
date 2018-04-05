#include "ds/rbtree.h"

static inline void rotate_right(struct rbtree *tree, struct rb_node *node)
{
	struct rb_node *child = node->left;

	node->left = child->right;
	if (node->left != NULL)
		rb_set_parent(node->left, node);

	rb_set_parent(child, rb_parent(node));

	if (rb_parent(node) == NULL)
		tree->root = child;
	else if (node == rb_parent(node)->left)
		rb_parent(node)->left = child;
	else
		rb_parent(node)->right = child;

	child->right = node;
	rb_set_parent(node, child);
}

static inline void rotate_left(struct rbtree *tree, struct rb_node *node)
{
	struct rb_node *child = node->right;

	node->right = child->left;
	if (node->right != NULL)
		rb_set_parent(node->right, node);

	rb_set_parent(child, rb_parent(node));

	if (rb_parent(node) == NULL)
		tree->root = child;
	else if (node == rb_parent(node)->left)
		rb_parent(node)->left = child;
	else
		rb_parent(node)->right = child;

	child->left = node;
	rb_set_parent(node, child);
}

static inline void insert_case1(struct rbtree *tree, struct rb_node *node)
{
	rb_set_color(node, RB_BLACK);
	tree->root = node;
}

static inline void insert_case2(struct rbtree *tree, struct rb_node *node)
{
	(void)node;
	(void)tree;
}

static inline void insert_case3(struct rbtree *tree, struct rb_node *node)
{
	rb_set_color(rb_parent(node), RB_BLACK);
	rb_set_color(rb_uncle(node), RB_BLACK);
	rb_set_color(rb_grandparent(node), RB_RED);
	rb_insert_color(tree, rb_grandparent(node));	
}

static inline void insert_case4(struct rbtree *tree, struct rb_node *node)
{
	struct rb_node *parent = rb_parent(node), *grandparent = rb_grandparent(node);

	if (grandparent->left && node == grandparent->left->right) {
		rotate_left(tree, parent);
		node = node->left;
	} else if (grandparent->right && node == grandparent->right->left) {
		rotate_right(tree, parent);
		node = node->right;
	}

	parent = rb_parent(node);
	grandparent = rb_grandparent(node);

	if (node == parent->left) {
		rotate_right(tree, grandparent);
	} else {
		rotate_left(tree, grandparent);
	}

	rb_set_color(parent, RB_BLACK);
	rb_set_color(grandparent, RB_RED);
}

void rb_insert_color(struct rbtree *tree, struct rb_node *node)
{
	if (node == NULL)
		return;

	struct rb_node *parent = rb_parent(node);
	if (parent == NULL)
		insert_case1(tree, node);
	else if (rb_color(parent) == RB_BLACK)
		insert_case2(tree, node);
	else if (rb_color(rb_uncle(node)) == RB_RED)
		insert_case3(tree, node);
	else
		insert_case4(tree, node);
}

void rb_erase(struct rbtree *tree, struct rb_node *node)
{
	(void)tree;
	(void)node;
}
