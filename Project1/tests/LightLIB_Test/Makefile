
ifeq ($(BB),black)
	BB_TYPE:="black"
else
	BB_TYPE:="green"
endif

ifeq ($(PLAT),BB)
	CC:=arm-linux-gnueabihf-gcc
else
	CC:=gcc
endif

CFLAGS:= -g3 -o0 -DBB_TYPE=$(BB_TYPE) #-std=gnu11
LDFLAGS:= -lpthread -lrt -lmraa -lm
INCLUDES:= -I ../../include/BeagleBone -I ../../include/common

OUTPUT_FILE_EXT:=.out
OUTPUT_FILE:= apds9301_test$(OUTPUT_FILE_EXT)

SRCS:= ../../src/my_i2c.c ../../src/apds9301_sensor.c

.PHONY:APDS9301_TEST
APDS9301_TEST:
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OUTPUT_FILE) apds9301_testmain.c $(SRCS) $(LDFLAGS)
	scp $(OUTPUT_FILE) gunj@192.168.7.2:~/

.PHONY:clean
clean:
	rm -f *.o $(OUTPUT_FILE)