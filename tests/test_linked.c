#include "minunit.h"
#include "linked.h"
#include <stdint.h>
#include <stdlib.h>

struct test_struct {
	int data;
	struct slist_entry next;
};

MU_TEST(entry) {
	struct test_struct t;
	struct test_struct *p = slist_entry(&t.next, struct test_struct, next);
	mu_assert_int_eq((uintptr_t)&t, (uintptr_t)p);
}

MU_TEST(get_next) {
	struct test_struct t = { 0, { NULL } };
	struct test_struct t2 = { 0, { NULL } };
	mu_assert_int_eq(0, (uintptr_t)slist_get_next(&t, next));
	t.next.next = &t2.next;
	mu_assert_int_eq((uintptr_t)&t2, (uintptr_t)slist_get_next(&t, next));
}

MU_TEST(set_next) {
	struct test_struct t = { 0, { NULL } };
	struct test_struct t2 = { 0, { NULL } };
	slist_set_next(&t, next, &t2);
	mu_assert_int_eq((uintptr_t)&t2, (uintptr_t)slist_get_next(&t, next));
	slist_set_next(&t, next, (struct test_struct *)NULL);
	mu_assert_int_eq(0, (uintptr_t)slist_get_next(&t, next));
}

MU_TEST(foreach) {
	struct test_struct t1 = { 0, { NULL } };
	struct test_struct t2 = { 1, { NULL } };
	struct test_struct t3 = { 2, { NULL } };
	struct test_struct t4 = { 3, { NULL } };
	slist_set_next(&t1, next, &t2);
	slist_set_next(&t2, next, &t3);
	slist_set_next(&t3, next, &t4);
	struct test_struct *current = &t1;
	int iterations = 0;
	slist_foreach(current, next) {
		mu_assert_int_eq(iterations++, current->data);
	}
}

MU_TEST_SUITE(linked) {
	MU_RUN_TEST(entry);
	MU_RUN_TEST(get_next);
	MU_RUN_TEST(set_next);
	MU_RUN_TEST(foreach);
}

int main(void) {
	MU_RUN_SUITE(linked);
	MU_REPORT();
}
