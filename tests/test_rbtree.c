#include "minunit.h"
#include "ds/rbtree.h"
#include <stdint.h>
#include <stdlib.h>
#include <setjmp.h>

struct test_node {
	struct rb_node node;
	int key;
};

static unsigned int __assert_is_balanced(struct rb_node *node, jmp_buf env)
{
	if (node == NULL)
		return 1;

	fprintf(stderr, "%p: %s - (%p, %p)\n", node, rb_is_red(node) ? "RED" : "BLACK", node->left, node->right);

	if (rb_is_red(node))
		if (!(rb_is_black(node->left) && rb_is_black(node->right)))
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
#define assert_is_balanced(tree) \
	({ \
	 	jmp_buf env; \
		int rv = setjmp(env); \
		switch (rv) { \
			case 0: break; \
			case 2: \
				mu_fail("Unbalanced tree - red parent with red child"); \
				break; \
			case 3: \
	 			mu_fail("Unbalanced tree - more black nodes on the left side of a node"); \
				break; \
			case 4: \
	 			mu_fail("Unbalanced tree - more black nodes on the right side of a node"); \
				break; \
			default: \
				mu_fail("Unbalanced tree - (unknown error)"); \
				break; \
		} \
		mu_check(rb_is_black((tree)->root)); \
		__assert_is_balanced((tree)->root, env); \
		fprintf(stderr, "---\n"); \
	})

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

MU_TEST(insert)
{
	struct rbtree tree = { 0 };
	struct test_node a = { .key = 1 }, b = { .key = 0 }, c = { .key = 2 };

	fprintf(stderr, "Insert test:\n---\n");

	tree.root = &a.node;
	rb_insert_color(&a.node);
	assert_is_balanced(&tree);

	tree.root->left = &b.node;
	rb_set_parent(&b.node, tree.root);
	rb_set_color(&b.node, RB_RED);
	rb_insert_color(&b.node);
	assert_is_balanced(&tree);

	tree.root->right = &c.node;
	rb_set_parent(&c.node, tree.root);
	rb_set_color(&c.node, RB_RED);
	rb_insert_color(&c.node);
	assert_is_balanced(&tree);
}

MU_TEST_SUITE(rbtree)
{
	MU_RUN_TEST(entry);
	MU_RUN_TEST(parent);
	MU_RUN_TEST(insert);
}

int main(void)
{
	MU_RUN_SUITE(rbtree);
	MU_REPORT();
}
