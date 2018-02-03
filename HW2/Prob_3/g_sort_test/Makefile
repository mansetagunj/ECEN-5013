BUILD_TIME=$(shell date +%s)

all:sysSortTest.out

g_sort.o:
	gcc -c g_sort.c


sysSortTest.out: g_sort.o
	gcc -o sysSortTest.out sysSortTest.c g_sort.c -DBUILD_TIME 


.PHONY:clean
clean:
	rm -rf *o *.out
