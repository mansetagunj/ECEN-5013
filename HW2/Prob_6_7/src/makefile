include sources.mk
	
TEST_EXE = test_doublyLL.out


CMOCKA_LIBRARY = ./../lib/libcmocka.a
LIB_PATH =  ./../lib/
LDFLAGS= -L$(LIB_PATH) -lcmocka
CC=gcc
CMOCKA_INCLUDE_DIR=-I ./../include/cmocka/
CFLAGS=-std=gnu11
INCLUDES=-I ./../include/common/
SRCS=$(SRCS_COMMON_C)
OBJS=$(SRCS:.c=.o)
TEST_OBJS = $(TEST_SRCS:.c=.o)

%.o: %.c
	@echo "Building $@ from $<"	
	$(CC) $(CFLAGS) $(CMOCKA_LIBRARY) $(INCLUDES) $(CMOCKA_INCLUDE_DIR) -c $< -o $@
	@echo "Build Finish\n"

.PHONY: compile-all
compile-all: $(OBJS) $(TEST_OBJS)

	
.PHONY: test
test : $(TEST_EXE)
	./$(TEST_EXE)

$(TEST_EXE) : $(TEST_OBJS) $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) $(CMOCKA_LIBRARY) $(CMOCKA_INCLUDE_DIR) -o $@ $^

.PHONY:clean
clean:
	rm -rf *.o
