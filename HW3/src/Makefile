CC=gcc


#ifeq ($(BUILD),RELEASE)
#output_type = _release
#output_dir= ../executable/release/
#else
#output_type = _debug
#output_dir= ../executable/debug/
#CFLAGS += -g3
#endif

output_dir= 
basename=pthreadHW
ext=.out
filename=$(basename)$(output_type)$(ext)

CFLAGS += -g3 -O0
LDFLAGS= -lpthread
INCLUDES= -I ../include -I ./

SRCS:= main.c time.c letterParser.c doublyLinkedList.c threadManager.c my_signals.c

OBJS=$(SRCS:.c=.o)

%.o: %.c
	@echo "Building $@ from $<"	
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	@echo "Build Finish\n"
	
.PHONY:build
build: $(OBJS)
	@echo "Generating $(filename)"	
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $(filename) $(LDFLAGS) 
	#mkdir -p $(output_dir)
	#cp $(filename) $(output_dir)$(filename)
	@echo "Generated $(filename) in $(output_dir)"

.PHONY:run
run:
	./$(output_dir)$(filename)

.PHONY:debug
debug:
	gdb -tui ./$(output_dir)$(filename)	
	

.PHONY:clean
clean:
	rm -rf *.o *.log *$(ext)
	@echo "Clean Complete"	

