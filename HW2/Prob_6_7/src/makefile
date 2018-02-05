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
TEST_OBJS =$(TEST_SRCS:.c=.o)

%.o: %.c
	@echo "Building $@ from $<"	
	$(CC) $(CFLAGS) $(CMOCKA_LIBRARY) $(INCLUDES) $(CMOCKA_INCLUDE_DIR) -c $< -o $@
	@echo "Build Finish\n"

.PHONY: compile-all
compile-all: $(OBJS) $(TEST_OBJS)

.PHONY:mytest
mytest: doublyLinkedList.o
	$(CC) $(CFLAGS) $(INCLUDES) -o my_test.out doublyLinkedList.c my_test.c
	
.PHONY: test
test : $(TEST_EXE)
	./$(TEST_EXE)

$(TEST_EXE) : doublyLinkedList.o test_list.o test.o
	#$(TEST_OBJS) $(OBJS) 
	$(CC) -std=c99 $(INCLUDES) $(CMOCKA_LIBRARY) $(CMOCKA_INCLUDE_DIR) -o $@ $^

#$(CMOCKA_LIBRARY) 

.PHONY:clean
clean:
	rm -rf *.o
