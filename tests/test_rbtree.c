#include "minunit.h"
#include "ds/rbtree.h"
#include <stdint.h>
#include <stdlib.h>
#include <setjmp.h>

#define TREE_SIZE 100

struct test_node {
	struct rb_node node;
	int key;
};

static unsigned int __assert_is_balanced(struct rb_node *node, jmp_buf env)
{
	if (node == NULL)
		return 1;

	if (rb_is_red(node))
		if (rb_is_red(node->left) || rb_is_red(node->right))
			longjmp(env, 2);

	unsigned int left_count = __assert_is_balanced(node->left, env);
	unsigned int right_count = __assert_is_balanced(node->right, env);

	if (rb_is_black(node))
		left_count++, right_count++;

	if (left_count > right_count)
		longjmp(env, 3);
	else if (left_count < right_count)
		longjmp(env, 4);

	// Otherwise, they are equal
	return left_count;
}

// It's not safe to go alone, take this! https://www.reddit.com/r/Eyebleach/
#define assert_is_balanced(tree, env) \
	do { \
		mu_check((tree)->root != NULL); \
		mu_check(rb_is_black((tree)->root)); \
		__assert_is_balanced((tree)->root, env); \
		fprintf(stderr, "---\n"); \
	} while (0)

static void __debug_tree(struct rb_node *node)
{
	if (node == NULL)
		return;
	
	fprintf(stderr, "%p: %s - (parent %p, left %p, right %p)\n", node, rb_is_red(node) ? "RED  " : "BLACK",
			rb_parent(node), node->left, node->right);
	__debug_tree(node->left);
	__debug_tree(node->right);
}

static void debug_tree(struct rbtree *tree)
{
	__debug_tree(tree->root);
}

static void test_tree_insert(struct rbtree *tree, struct test_node *new_node)
{
	struct rb_node **link = &tree->root, *parent = NULL;

	while (*link) {
		struct test_node *this = rb_entry(*link, struct test_node, node);
		parent = *link;

		if (this->key < new_node->key)
			link = &(*link)->left;
		else
			link = &(*link)->right;
	}

	rb_link_node(&new_node->node, parent, link);
	rb_insert_color(tree, &new_node->node);
	debug_tree(tree);
}

MU_TEST(entry)
{
	struct test_node t;
	struct test_node *p = rb_entry(&t.node, struct test_node, node);
	mu_assert_int_eq((uintptr_t)&t, (uintptr_t)p);

	// Test the safe version (which properly handles NULLs)
	rb_entry_safe(&t.node, struct test_node, node);
	mu_assert_int_eq((uintptr_t)&t, (uintptr_t)p);
	mu_assert_int_eq(0, (uintptr_t)rb_entry_safe(NULL, struct test_node, node));
}

MU_TEST(parent)
{
	struct test_node parent, child;
	parent.node.left = &child.node;
	child.node.parent_color = rb_mask_parent(&parent.node);
	mu_assert_int_eq((uintptr_t)rb_parent(&child.node), (uintptr_t)&parent.node);
}

MU_TEST(rblink)
{
	struct test_node a = { 0 }, b = { 0 };

	rb_link_node(&b.node, &a.node, &a.node.left);
	mu_assert_int_eq((uintptr_t)a.node.left, (uintptr_t)&b.node);
	mu_assert_int_eq((uintptr_t)rb_parent(&b.node), (uintptr_t)&a.node);
	mu_assert_int_eq(0, (uintptr_t)b.node.left);
	mu_assert_int_eq(0, (uintptr_t)b.node.right);
}

MU_TEST(insert_and_erase)
{
	struct rbtree tree = { 0 };
	struct test_node *nodes[TREE_SIZE];

	fprintf(stderr, "Insert test:\n---\n");

	jmp_buf env;
	int rv = setjmp(env);
	switch (rv) {
		case 0: break;
		case 2:
			mu_fail("Unbalanced tree - red parent with red child");
			break;
		case 3:
			mu_fail("Unbalanced tree - more black nodes on the left side of a node");
			break;
		case 4:
			mu_fail("Unbalanced tree - more black nodes on the right side of a node");
			break;
		default:
			mu_fail("Unbalanced tree - (unknown error)");
			break;
	}

	for (size_t i = 0; i < TREE_SIZE; i++) {
		struct test_node *n = calloc(1, sizeof *n);
		nodes[i] = n;
		n->key = rand();
		test_tree_insert(&tree, n);
		assert_is_balanced(&tree, env);
	}

	for (size_t i = 0; i < TREE_SIZE; i++) {
		//rb_erase(&tree, &nodes[i]->node);
		//assert_is_balanced(&tree, env);
		free(nodes[i]);
	}
}

MU_TEST_SUITE(rbtree)
{
	MU_RUN_TEST(entry);
	MU_RUN_TEST(parent);
	MU_RUN_TEST(rblink);
	MU_RUN_TEST(insert_and_erase);
}

int main(void)
{
	MU_RUN_SUITE(rbtree);
	MU_REPORT();
}
