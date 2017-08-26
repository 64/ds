#include "minunit.h"
#include "bitmap.h"

MU_TEST(test_check) {
	mu_check(export_fn() == -5);
}

MU_TEST_SUITE(test_suite) {
	MU_RUN_TEST(test_check);
}

int main(void) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
}
