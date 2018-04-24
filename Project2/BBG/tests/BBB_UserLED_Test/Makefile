
ifeq ($(BB),black)
	BB_TYPE:="black"
else
	BB_TYPE:="green"
endif

CC_CROSS:=arm-linux-gnueabihf-gcc
CC:=gcc
CFLAGS:= -g3 -O0 -DBB_TYPE=$(BB_TYPE) #-std=gnu11
LDFLAGS:= -lpthread -lrt
INCLUDES:= -I ../../include/BeagleBone

OUTPUT_FILE_EXT:=.out
OUTPUT_FILE:= led_test$(OUTPUT_FILE_EXT)

.PHONY:LED_TEST
LED_TEST:
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OUTPUT_FILE) LED_test.c ../../src/BB_Led.c
	
.PHONY:cross
cross:
	$(CC_CROSS) $(CFLAGS) $(INCLUDES) -o $(OUTPUT_FILE) LED_test.c ../../src/BB_Led.c
	scp $(OUTPUT_FILE) gunj@192.168.7.2:~/

.PHONY:clean
clean:
	rm -f *.o $(OUTPUT_FILE)