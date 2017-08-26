.PHONY: all test clean
.SUFFIXES: .o .c .test

CFLAGS += -Wall -Wextra -Wpedantic -Werror -std=gnu11 -Iinclude

OBJ_LIST := \
bitmap.o
OBJS = $(addprefix build/,$(OBJ_LIST))

TEST_EXE_LIST := \
test_bitmap
TEST_EXES = $(addprefix build/,$(TEST_EXE_LIST))

all: build/libds.a

test: build/libds.a $(TEST_EXES)
	@./tests/run_tests.sh $(TEST_EXES)

clean:
	rm -rf build/

build/:
	mkdir build

build/libds.a: build/ $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

build/%.o: src/%.c
	$(CC) -c $< -o $@ -MD $(CFLAGS)

build/%: tests/%.c
	$(CC) $< -o $@ -Lbuild -lds -MD $(CFLAGS)

-include build/*.d
