include sources.mk
	
TEST_OBJS = $(TEST_SRCS:%.c=%.o)

TEST_EXE = test_doublyLL.out


CMOCKA_LIBRARY = ./../lib/libcmocka.a

CC=gcc
CMOCKA_INCLUDE_DIR=-I ./../include/cmocka/
CFLAGS=-std=c99
INCLUDES=-I ./../include/common/
SRCS=$(SRCS_COMMON_C)

OBJS=$(SRCS:.c=.o)

%.o: %.c
	@echo "Building $@ from $<"	
	$(CC) $(CFLAGS) $(INCLUDES) $(CMOCKA_INCLUDE_DIR) -c $< -o $@
	@echo "Build Finish\n"

.PHONY: compile-all
compile-all: $(OBJS)

.PHONY: build
build: 	PRINT_PLATFORM $(OBJS) $(DEPS) 
	@echo "Building and Linking. Generating - dllist.out \n" 
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(LINKER) $(INCLUDES) $(OBJS) -o dllist.out
	@echo "Build and Link Complete\n"
	
.PHONY: test
test : $(TEST_EXE)
	./$(TEST_EXE)

$(TEST_EXE) : $(TEST_OBJS) $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(CMOCKA_INCLUDE_DIR) -o $@ $^ $(CMOCKA_LIBRARY) -lm
