#include "minunit.h"
#include "ds/hash.h"

struct test_item {
	int data;
	struct dlist_node node;
};

MU_TEST(add)
{
	struct hlist_bucket bucket = { .head = NULL };
	struct test_item t1 = { .data = 0 };
	struct test_item t2 = { .data = 1 };
	struct test_item t3 = { .data = 2 };
	
	hlist_add(&bucket, &t1.node);
	mu_check(bucket.head == &t1.node);
	mu_check(bucket.head->prev == NULL);
	mu_check(bucket.head->next == NULL);

	hlist_add(&bucket, &t2.node);
	mu_check(bucket.head == &t2.node);
	mu_check(bucket.head->prev == NULL);
	mu_check(bucket.head->next == &t1.node);
	mu_check(bucket.head->next->prev == &t2.node);
	mu_check(bucket.head->next->next == NULL);

	hlist_add(&bucket, &t3.node);
	mu_check(bucket.head == &t3.node);
	mu_check(bucket.head->prev == NULL);
	mu_check(bucket.head->next == &t2.node);
	mu_check(bucket.head->next->prev == &t3.node);
	mu_check(bucket.head->next->next == &t1.node);
	mu_check(bucket.head->next->next->prev == &t2.node);
	mu_check(bucket.head->next->next->next == NULL);
}

MU_TEST(hremove)
{
	struct hlist_bucket bucket = { .head = NULL };
	struct test_item t1 = { .data = 0 };
	struct test_item t2 = { .data = 1 };
	struct test_item t3 = { .data = 2 };

	hlist_add(&bucket, &t1.node);
	hlist_add(&bucket, &t2.node);
	hlist_add(&bucket, &t3.node);

	// Remove from middle
	hlist_remove(&bucket, &t2.node);
	mu_check(bucket.head == &t3.node);
	mu_check(bucket.head->prev == NULL);
	mu_check(bucket.head->next == &t1.node);
	mu_check(bucket.head->next->prev == &t3.node);
	mu_check(bucket.head->next->next == NULL);

	// Remove from tail
	hlist_remove(&bucket, &t1.node);
	mu_check(bucket.head == &t3.node);
	mu_check(bucket.head->prev == NULL);
	mu_check(bucket.head->next == NULL);

	hlist_add(&bucket, &t1.node);

	// Remove from head with 1 behind
	hlist_remove(&bucket, &t1.node);
	mu_check(bucket.head == &t3.node);
	mu_check(bucket.head->prev == NULL);
	mu_check(bucket.head->next == NULL);

	// Remove from head with 0 behind
	hlist_remove(&bucket, &t3.node);
	mu_check(bucket.head == NULL);
}

MU_TEST(foreach)
{
	struct hlist_bucket bucket = { .head = NULL };
	struct test_item t1 = { .data = 0 };
	struct test_item t2 = { .data = 1 };
	struct test_item t3 = { .data = 2 };

	hlist_add(&bucket, &t3.node);
	hlist_add(&bucket, &t2.node);
	hlist_add(&bucket, &t1.node);

	int i = 0;
	hlist_foreach(item, struct test_item, node, &bucket) {
		mu_assert_int_eq(i, item->data);
		i++;	
	}

	mu_assert_int_eq(3, i);
}

MU_TEST_SUITE(hash)
{
	MU_RUN_TEST(add);
	MU_RUN_TEST(hremove);
	MU_RUN_TEST(foreach);
}

int main(void)
{
	MU_RUN_SUITE(hash);
	MU_REPORT();
}
