

.PHONY:default
default:
	@echo "Build Started"
	gcc -g3 -I./../../include/common/ client.c -o client -lpthread
	@echo "Build Completed"

.PHONY:cross
cross:
	@echo "Build Started"
	arm-linux-gnueabihf-gcc -g3 -I./../../include/common/ client.c -o client -lpthread
	@echo "Build Completed"
	scp client gunj@192.168.7.2:~/

.PHONY:clean
clean:
	rm -rf client 
