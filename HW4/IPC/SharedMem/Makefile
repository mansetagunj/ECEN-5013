

.PHONY:default
default:
	@echo "Build Started"
	gcc -g3 process1.c -o proc1 -lpthread -lrt
	gcc -g3 process2.c -o proc2 -lpthread -lrt
	@echo "Build Completed"

.PHONY:clean
clean:
	rm -rf proc1 proc2
