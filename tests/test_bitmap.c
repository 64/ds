#include "minunit.h"
#include "ds/bitmap.h"
#include <stdlib.h>

#define TEST_BYTES 13
#define TEST_BITS (TEST_BYTES * 8)

MU_TEST(test) {
	unsigned char *data = calloc(1, TEST_BYTES);
	for (int i = 0; i < TEST_BITS; i++) {
		mu_assert_int_eq(0, bitmap_test(data, i));
	}

	memset(data, 0xFF, TEST_BYTES);
	for (int i = 0; i < TEST_BITS; i++) {
		mu_assert_int_neq(0, bitmap_test(data, i));
	}

	data[2] &= ~(1 << 4);
	mu_assert_int_eq(0, bitmap_test(data, 2 * 8 + 4));
	mu_assert_int_neq(0, bitmap_test(data, 2 * 8 + 5));
	free(data);
}

MU_TEST(set) {
	unsigned char *data = calloc(1, TEST_BYTES);
	for (int i = 0; i < TEST_BITS; i++) {
		bitmap_set(data, i);
		mu_assert_int_neq(0, bitmap_test(data, i));
	}
	free(data);
}

MU_TEST(clear) {
	unsigned char *data = malloc(TEST_BYTES);
	memset(data, 0xFF, TEST_BYTES);
	for (int i = 0; i < TEST_BITS; i++) {
		bitmap_clear(data, i);
		mu_assert_int_eq(0, bitmap_test(data, i));
	}
	free(data);
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
	free(data);
}

MU_TEST(bwrite) {
	unsigned char *data = malloc(TEST_BYTES);
	memset(data, 0xFF, TEST_BYTES);
	for (int i = 0; i < TEST_BITS; i++) {
		bitmap_write(data, i, i % 2);
		if (i % 2)
			mu_assert_int_neq(0, bitmap_test(data, i));
		else
			mu_assert_int_eq(0, bitmap_test(data, i));
	}
	free(data);
}

MU_TEST(find_hole) {
	unsigned char *data = malloc(TEST_BYTES);
	memset(data, 0xFF, TEST_BYTES);
	mu_assert_int_eq(-1, bitmap_find_hole(data, TEST_BITS, 1));
	data[0] = 0x7F;
	mu_assert_int_eq(7, bitmap_find_hole(data, TEST_BITS, 1));
	data[0] = 0xFF;
	data[1] = 0x8F;
	mu_assert_int_eq(-1, bitmap_find_hole(data, TEST_BITS, 4));
	mu_assert_int_eq(12, bitmap_find_hole(data, TEST_BITS, 3));
	data[1] = 0x0F;
	data[2] = 0xF0;
	mu_assert_int_eq(12, bitmap_find_hole(data, TEST_BITS, 8));
	free(data);
}

MU_TEST_SUITE(bitmap) {
	MU_RUN_TEST(test);
	MU_RUN_TEST(set);
	MU_RUN_TEST(clear);
	MU_RUN_TEST(toggle);
	MU_RUN_TEST(bwrite);
	MU_RUN_TEST(find_hole);
}

int main(void) {
	MU_RUN_SUITE(bitmap);
	MU_REPORT();
}
