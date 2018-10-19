#include "minunit.h"
#include "ds/linked.h"
#include <stdint.h>
#include <stdlib.h>

struct test_slist {
	int data;
	struct slist_node list;
};

struct test_dlist {
	int data;
	struct dlist_node list;
};

MU_TEST(entry) {
	struct test_slist t1_s;
	struct test_slist *p_s = slist_entry(&t1_s.list, struct test_slist, list);
	mu_assert_int_eq((uintptr_t)&t1_s, (uintptr_t)p_s);

	struct test_dlist t1_d;
	struct test_dlist *p_d = dlist_entry(&t1_d.list, struct test_dlist, list);
	mu_assert_int_eq((uintptr_t)&t1_d, (uintptr_t)p_d);
}

MU_TEST(get_next) {
	struct test_slist t1_s = { 0, { NULL } };
	struct test_slist t2_s = { 0, { NULL } };
	mu_assert_int_eq(0, (uintptr_t)slist_get_next(&t1_s, list));
	t1_s.list.next = &t2_s.list;
	mu_assert_int_eq((uintptr_t)&t2_s, (uintptr_t)slist_get_next(&t1_s, list));

	struct test_dlist t1_d = { 0, { NULL, NULL } };
	struct test_dlist t2_d = { 0, { NULL, NULL } };
	mu_assert_int_eq(0, (uintptr_t)dlist_get_next(&t1_d, list));
	t1_d.list.next = &t2_d.list;
	mu_assert_int_eq((uintptr_t)&t2_d, (uintptr_t)dlist_get_next(&t1_d, list));
}

MU_TEST(get_prev) {
	struct test_dlist t1_d = { 0, { NULL, NULL } };
	struct test_dlist t2_d = { 0, { NULL, NULL } };
	mu_assert_int_eq(0, (uintptr_t)dlist_get_prev(&t2_d, list));
	t2_d.list.prev = &t1_d.list;
	mu_assert_int_eq((uintptr_t)&t1_d, (uintptr_t)dlist_get_prev(&t2_d, list));
}

MU_TEST(set_next) {
	struct test_slist t1_s = { 0, { NULL } };
	struct test_slist t2_s = { 0, { NULL } };
	slist_set_next(&t1_s, list, &t2_s);
	mu_assert_int_eq((uintptr_t)&t2_s, (uintptr_t)slist_get_next(&t1_s, list));
	slist_set_next(&t1_s, list, (struct test_slist *)NULL);
	mu_assert_int_eq(0, (uintptr_t)slist_get_next(&t1_s, list));

	struct test_dlist t1_d = { 0, { NULL, NULL } };
	struct test_dlist t2_d = { 0, { NULL, NULL } };
	dlist_set_next(&t1_d, list, &t2_d);
	mu_assert_int_eq((uintptr_t)&t2_d, (uintptr_t)dlist_get_next(&t1_d, list));
	dlist_set_next(&t1_d, list, (struct test_dlist *)NULL);
	mu_assert_int_eq(0, (uintptr_t)dlist_get_next(&t1_d, list));
}

MU_TEST(set_prev) {
	struct test_dlist t1_d = { 0, { NULL, NULL } };
	struct test_dlist t2_d = { 0, { NULL, NULL } };
	dlist_set_prev(&t2_d, list, &t1_d);
	mu_assert_int_eq((uintptr_t)&t1_d, (uintptr_t)dlist_get_prev(&t2_d, list));
	dlist_set_prev(&t2_d, list, (struct test_dlist *)NULL);
	mu_assert_int_eq(0, (uintptr_t)dlist_get_prev(&t2_d, list));
}

MU_TEST(foreach) {
	struct test_slist t1_s = { 0, { NULL } };
	struct test_slist t2_s = { 1, { NULL } };
	struct test_slist t3_s = { 2, { NULL } };
	struct test_slist t4_s = { 3, { NULL } };
	slist_set_next(&t1_s, list, &t2_s);
	slist_set_next(&t2_s, list, &t3_s);
	slist_set_next(&t3_s, list, &t4_s);
	int iterations_s = 0;
	slist_foreach(current_s, list, &t1_s) {
		mu_assert_int_eq(iterations_s++, current_s->data);
	}

	struct test_dlist t1_d = { 0, { NULL, NULL } };
	struct test_dlist t2_d = { 1, { NULL, NULL } };
	struct test_dlist t3_d = { 2, { NULL, NULL } };
	struct test_dlist t4_d = { 3, { NULL, NULL } };
	dlist_set_next(&t1_d, list, &t2_d);
	dlist_set_next(&t2_d, list, &t3_d);
	dlist_set_next(&t3_d, list, &t4_d);
	int iterations_d = 0;
	dlist_foreach(current_d, list, &t1_d) {
		mu_assert_int_eq(iterations_d++, current_d->data);
	}
}

MU_TEST(append) {
	struct test_slist t1_s = { 0, { NULL } };
	struct test_slist t2_s = { 0, { NULL } };
	struct test_slist t3_s = { 0, { NULL } };
	slist_append(&t1_s, list, &t2_s);
	slist_append(&t1_s, list, &t3_s);
	mu_assert_int_eq((uintptr_t)&t2_s, (uintptr_t)slist_get_next(&t1_s, list));
	mu_assert_int_eq((uintptr_t)&t3_s, (uintptr_t)slist_get_next(&t2_s, list));

	struct test_dlist t1_d = { 0, { NULL, NULL } };
	struct test_dlist t2_d = { 0, { NULL, NULL } };
	struct test_dlist t3_d = { 0, { NULL, NULL } };
	dlist_append(&t1_d, list, &t2_d);
	dlist_append(&t1_d, list, &t3_d);
	mu_assert_int_eq((uintptr_t)&t2_d, (uintptr_t)dlist_get_next(&t1_d, list));
	mu_assert_int_eq((uintptr_t)&t3_d, (uintptr_t)dlist_get_next(&t2_d, list));
}

MU_TEST(dremove) {
	struct test_dlist t1_d = { 0, { NULL, NULL } };
	struct test_dlist t2_d = { 0, { NULL, NULL } };
	struct test_dlist t3_d = { 0, { NULL, NULL } };
	dlist_set_next(&t1_d, list, &t2_d);
	dlist_set_next(&t2_d, list, &t3_d);
	mu_assert_int_eq((uintptr_t)&t2_d, (uintptr_t)dlist_get_next(&t1_d, list));
	dlist_remove(&t2_d, list);
	mu_assert_int_eq((uintptr_t)&t3_d, (uintptr_t)dlist_get_next(&t1_d, list));
}

MU_TEST_SUITE(linked) {
	MU_RUN_TEST(entry);
	MU_RUN_TEST(get_next);
	MU_RUN_TEST(get_prev);
	MU_RUN_TEST(set_next);
	MU_RUN_TEST(set_prev);
	MU_RUN_TEST(foreach);
	MU_RUN_TEST(append);
	MU_RUN_TEST(dremove);
}

int main(void) {
	MU_RUN_SUITE(linked);
	MU_REPORT();
}
