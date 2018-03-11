

.PHONY:default
default:
	@echo "Build Started"
	gcc -g3 client.c -o client
	@echo "Build Completed"

.PHONY:cross
cross:
	@echo "Build Started"
	arm-linux-gnueabihf-gcc -g3 client.c -o client
	@echo "Build Completed"
	scp client ubuntu@192.168.7.2:~/

.PHONY:clean
clean:
	rm -rf client 
