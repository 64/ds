#include "minunit.h"
#include "ds/rbtree.h"
#include <stdint.h>
#include <stdlib.h>
#include <setjmp.h>

#define TREE_SIZE 1000

struct test_node {
	struct rb_node node;
	int key;
};

// It's not safe to go alone, take this! https://www.reddit.com/r/Eyebleach/
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

	// Check for corrupt parent pointers
	if (node->left && rb_parent(node->left) != node)
		longjmp(env, 5);
	if (node->right && rb_parent(node->right) != node)
		longjmp(env, 5);

	struct test_node *self, *left, *right;
	self = rb_entry(node, struct test_node, node);
	left = rb_entry(node->left, struct test_node, node);
	right = rb_entry(node->right, struct test_node, node);
	if (left && left->key > self->key)
		longjmp(env, 6);
	if (right && right->key < self->key)
		longjmp(env, 6);

	// Otherwise, they are equal
	return left_count;
}

#define assert_is_balanced(tree, env) \
	do { \
		mu_check((tree)->root != NULL); \
		mu_check(rb_is_black((tree)->root)); \
		mu_check(rb_first_uncached((tree)) == rb_first_cached((tree))); \
		mu_assert_int_eq((uintptr_t)rb_first_uncached((tree))->left, 0); \
		mu_assert_int_eq((uintptr_t)rb_first_cached((tree))->left, 0); \
		__assert_is_balanced((tree)->root, env); \
	} while (0)

static void __debug_tree(struct rb_node *node)
{
	if (node == NULL)
		return;
	
	fprintf(stderr, "%-18p: %s - (parent %-18p, left %-18p, right %-18p): %10d\n", node, rb_is_red(node) ? "RED  " : "BLACK",
			rb_parent(node), node->left, node->right, rb_entry(node, struct test_node, node)->key);
	__debug_tree(node->left);
	__debug_tree(node->right);
}

static void __attribute__((unused)) debug_tree(struct rbtree *tree)
{
	__debug_tree(tree->root);
	fprintf(stderr, "---\n");
}

static void test_tree_insert(struct rbtree *tree, struct test_node *new_node)
{
	struct rb_node **link = &tree->root, *parent = NULL;

	while (*link) {
		struct test_node *this = rb_entry(*link, struct test_node, node);
		parent = *link;

		if (new_node->key < this->key)
			link = &(*link)->left;
		else
			link = &(*link)->right;
	}

	struct test_node *first = rb_entry(rb_first_uncached(tree), struct test_node, node);

	rb_link_node(&new_node->node, parent, link);
	rb_insert(tree, &new_node->node, first == NULL || new_node->key < first->key);
}

MU_TEST(entry)
{
	struct test_node t;
	struct test_node *p = rb_entry(&t.node, struct test_node, node);
	mu_assert_int_eq((uintptr_t)&t, (uintptr_t)p);
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

MU_TEST(replace)
{
	struct rbtree tree = { 0 };
	struct test_node a = { .key = 'a' }, b = { .key = 'b' }, c = { .key = 'c' };

	tree.root = &a.node;
	a.node.right = &b.node;
	rb_set_parent(&b.node, &a.node);
	rb_set_color(&a.node, 1);

	rb_replace(&tree, &a.node, &c.node);
	struct test_node *t = rb_entry(tree.root, struct test_node, node);
	mu_assert_int_eq((uintptr_t)t, (uintptr_t)&c);
	mu_assert_int_eq(t->key, 'c');
	mu_assert_int_eq((uintptr_t)c.node.left, 0);
	mu_assert_int_eq((uintptr_t)c.node.right, (uintptr_t)&b.node);
	mu_assert_int_eq(rb_color(&c.node), rb_color(&a.node));
}

MU_TEST(insert_and_erase)
{
	struct rbtree tree = { 0 };
	struct test_node *nodes[TREE_SIZE];

	fprintf(stderr, "Insert test:\n---\n");
	srand(time(NULL));

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
		case 5:
			mu_fail("Unbalanced tree - corrupt parent pointers");
			break;
		case 6:
			mu_fail("Unbalanced tree - invalid key ordering");
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

	// Test whether rb_next works
	struct rb_node *n = rb_first_uncached(&tree);
	while (rb_next(n) != NULL) {
		int prevkey = rb_entry(n, struct test_node, node)->key;
		n = rb_next(n);
		int curkey = rb_entry(n, struct test_node, node)->key;
		mu_check(prevkey <= curkey);
	}

	for (size_t i = 0; i < TREE_SIZE; i++) {
		assert_is_balanced(&tree, env);
		rb_erase(&tree, &nodes[i]->node);
	}

	for (size_t i = 0; i < TREE_SIZE; i++)
		free(nodes[i]);

	mu_check(tree.root == NULL);
}

MU_TEST_SUITE(rbtree)
{
	MU_RUN_TEST(entry);
	MU_RUN_TEST(parent);
	MU_RUN_TEST(rblink);
	MU_RUN_TEST(replace);
	MU_RUN_TEST(insert_and_erase);
}

int main(void)
{
	MU_RUN_SUITE(rbtree);
	MU_REPORT();
}
