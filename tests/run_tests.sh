#!/bin/sh

COLOR_RESET="\033[0m"
COLOR_RED="\033[0;31m"
COLOR_GREEN="\033[0;32m"

num_failed=0
num_successful=0
for test in $@; do
	./$test > temp_test_out.log
	if [ $? -ne 0 ] ; then
		printf "$COLOR_RED------- Test failure in $test: -------$COLOR_RESET\n"
		cat temp_test_out.log
		num_failed=$(expr $num_failed + 1)
	else
		num_successful=$(expr $num_successful + 1)
	fi
done

rm -f temp_test_out.log
if [ $num_failed -ne 0 ] ; then
	printf "$COLOR_RED%d test(s) failed.$COLOR_RESET\n" $num_failed
	exit 1
elif [ $num_successful -eq 0 ] ; then
	printf "$COLOR_RED%s$COLOR_RESET\n" "No tests found!"
	exit 1
fi

printf "$COLOR_GREEN%s$COLOR_RESET\n" "All tests passed."
exit 0
