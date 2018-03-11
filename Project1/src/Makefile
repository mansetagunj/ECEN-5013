include sources.mk

ifeq ($(BB),black)
	BB_TYPE:=black
else
	BB_TYPE:=green
endif

CC:=gcc
CFLAGS:= -g3 -o0 -DBB_TYPE=$(BB_TYPE) #-std=gnu11
LDFLAGS:= -lpthread -lrt
INCLUDES:= -I ../include/common -I ../include/cmocka -I ../include/BeagleBone

OUTPUT_FILE_EXT:=.out
OUTPUT_FILE:= project1$(OUTPUT_FILE_EXT)


.PHONY:build
build:
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OUTPUT_FILE) $(SRCS) $(LDFLAGS)

.PHONY:run
run: build
	sudo ./project1.out


.PHONY:clean
clean:
	rm -f *.o $(OUTPUT_FILE)