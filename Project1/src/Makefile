include sources.mk

ifeq ($(BB),black)
	BB_TYPE:=black
else
	BB_TYPE:=green
endif

CC:=gcc
CC_CROSS:=arm-linux-gnueabihf-gcc
CFLAGS:= -g3 -o0 -DBB_TYPE=$(BB_TYPE) #-std=gnu11
LDFLAGS:= -lpthread -lrt -lmraa -lm
INCLUDES:= -I ../include/common -I ../include/cmocka -I ../include/BeagleBone

OUTPUT_FILE_EXT:=.out

OUTPUT_FILE:= project1$(OUTPUT_FILE_EXT)


.PHONY:build
build:
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OUTPUT_FILE) $(SRCS) $(LDFLAGS)

.PHONY:cross
cross:
	$(CC_CROSS) $(CFLAGS) $(INCLUDES) -o BB_$(OUTPUT_FILE) $(SRCS) $(LDFLAGS)
	scp BB_$(OUTPUT_FILE) ubuntu@192.168.7.2:~/

.PHONY:run
run: build
	sudo ./project1.out


.PHONY:clean
clean:
	rm -f *.o *.log $(OUTPUT_FILE) BB_$(OUTPUT_FILE)