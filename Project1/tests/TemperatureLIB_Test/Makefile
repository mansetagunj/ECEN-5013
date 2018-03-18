
ifeq ($(BB),black)
	BB_TYPE:="black"
else
	BB_TYPE:="green"
endif

CC_CROSS:=arm-linux-gnueabihf-gcc
CC:=gcc
CFLAGS:= -O0 -DBB_TYPE=$(BB_TYPE) -g3  #-std=gnu11
LDFLAGS:= -pthread -lrt -lmraa -lm -lcmocka
INCLUDES:= -I ../../include/BeagleBone -I ../../include/cmocka

OUTPUT_FILE_EXT:=.out
OUTPUT_FILE:= tmp102_test$(OUTPUT_FILE_EXT)

SRCS:= ../../src/my_i2c.c ../../src/tmp102_sensor.c

.PHONY:TMP102_TEST
TMP102_TEST:
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OUTPUT_FILE) tmp102_testmain.c $(SRCS) $(LDFLAGS)

.PHONY:cross
cross:
	$(CC_CROSS) $(CFLAGS) $(INCLUDES) -o $(OUTPUT_FILE) tmp102_testmain.c $(SRCS) $(LDFLAGS)
	scp $(OUTPUT_FILE) gunj@192.168.7.2:~/

.PHONY:clean
clean:
	rm -f *.o $(OUTPUT_FILE)
