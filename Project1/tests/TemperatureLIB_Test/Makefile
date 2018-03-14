
ifeq ($(BB),black)
	BB_TYPE:="black"
else
	BB_TYPE:="green"
endif

CC:=arm-linux-gnueabihf-gcc
CFLAGS:= -g3 -o0 -DBB_TYPE=$(BB_TYPE) #-std=gnu11
LDFLAGS:= -lpthread -lrt -lmraa -lm
INCLUDES:= -I ../../include/BeagleBone

OUTPUT_FILE_EXT:=.out
OUTPUT_FILE:= tmp102_test$(OUTPUT_FILE_EXT)

SRCS:= ../../src/my_i2c.c ../../src/tmp102_sensor.c

.PHONY:LED_TEST
LED_TEST:
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OUTPUT_FILE) tmp102_testmain.c $(SRCS) $(LDFLAGS)
	scp $(OUTPUT_FILE) gunj@192.168.7.2:~/

.PHONY:clean
clean:
	rm -f *.o $(OUTPUT_FILE)