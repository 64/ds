#include "minunit.h"
#include "bitmap.h"
#include <stdlib.h>

#define TEST_BYTES 15
#define TEST_BITS (15 * 8)

MU_TEST(test) {
	unsigned char *data = calloc(1, TEST_BYTES);
	for (int i = 0; i < TEST_BITS; i++) {
		mu_assert_int_eq(bitmap_test(data, i), 0);
	}

	memset(data, 0xFF, TEST_BYTES);
	for (int i = 0; i < TEST_BITS; i++) {
		mu_assert_int_neq(bitmap_test(data, i), 0);
	}

	data[2] &= ~(1 << 4);
	mu_assert_int_eq(bitmap_test(data, 2 * 8 + 4), 0);
	mu_assert_int_neq(bitmap_test(data, 2 * 8 + 5), 0);
}

MU_TEST(set) {
	unsigned char *data = calloc(1, TEST_BYTES);
	for (int i = 0; i < TEST_BITS; i++) {
		bitmap_set(data, i);
		mu_assert_int_neq(bitmap_test(data, i), 0);
	}
}

MU_TEST(clear) {
	unsigned char *data = malloc(TEST_BYTES);
	memset(data, 0xFF, TEST_BYTES);
	for (int i = 0; i < TEST_BITS; i++) {
		bitmap_clear(data, i);
		mu_assert_int_eq(bitmap_test(data, i), 0);
	}
}

MU_TEST(toggle) {
	unsigned char *data = malloc(TEST_BYTES);
	memset(data, 0xAA, TEST_BYTES);
	for (int i = 0; i < TEST_BITS; i++) {
		bool before = bitmap_test(data, i);
		bitmap_toggle(data, i);
		bool after = bitmap_test(data, i);
		mu_assert_int_neq(before, after);
	}
}

MU_TEST(bwrite) {
	unsigned char *data = malloc(TEST_BYTES);
	memset(data, 0xFF, TEST_BYTES);
	for (int i = 0; i < TEST_BITS; i++) {
		bitmap_write(data, i, i % 2);
		if (i % 2)
			mu_assert_int_neq(bitmap_test(data, i), 0);
		else
			mu_assert_int_eq(bitmap_test(data, i), 0);
	}
}

MU_TEST_SUITE(bitmap) {
	MU_RUN_TEST(test);
	MU_RUN_TEST(set);
	MU_RUN_TEST(clear);
	MU_RUN_TEST(toggle);
	MU_RUN_TEST(bwrite);
}

int main(void) {
	MU_RUN_SUITE(bitmap);
	MU_REPORT();
}
