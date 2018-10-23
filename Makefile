.PHONY: all test clean build-tests
.SUFFIXES: .o .c .test

CFLAGS += -Wall -Wextra -Werror -std=gnu11 -Iinclude
DEBUG_CFLAGS := -g -Og
RELEASE_CFLAGS := -Ofast

OBJ_LIST := \
bitmap.o \
rbtree.o
OBJS = $(addprefix build/,$(OBJ_LIST))

TEST_EXE_LIST := \
test_bitmap \
test_linked \
test_rbtree \
test_hash
TEST_EXES = $(addprefix build/,$(TEST_EXE_LIST))

ifeq ($(DEBUG),0)
    CFLAGS += $(RELEASE_CFLAGS)
else
    CFLAGS += $(DEBUG_CFLAGS)
endif

ifeq ($(HOST),this)
    CFLAGS += -fsanitize=address,undefined
else
    CFLAGS += -fsanitize=undefined
endif

all: build/libds.a

test: build/libds.a $(TEST_EXES)
	@./tests/run_tests.sh $(TEST_EXES)

build-tests: build/libds.a $(TEST_EXES)

clean:
	rm -rf build/

build/:
	mkdir build

build/libds.a: build/ $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

build/%.o: src/%.c
	$(CC) -c $< -o $@ -MD $(CFLAGS)

build/%: tests/%.c build/libds.a
	$(CC) $< -o $@ -Lbuild -lds -MD $(CFLAGS)

-include build/*.d
